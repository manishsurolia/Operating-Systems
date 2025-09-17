#define _GNU_SOURCE
#include <sched.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#define SNAP_PATH "/mnt/snap_test"

static void read_and_print_hostname(const char *path, const char *who) {
    FILE *f = fopen(path, "r");
    if (!f) {
        perror("fopen");
        return;
    }
    char buf[256];
    if (fgets(buf, sizeof(buf), f) != NULL) {
        // remove trailing newline
        buf[strcspn(buf, "\n")] = 0;
        printf("[%s] /etc/hostname contains: '%s'\n", who, buf);
    } else {
        printf("[%s] Could not read hostname string\n", who);
    }
    fclose(f);
}

int main(void) {
    int status;

    // Parent: read /etc/hostname before snapshot
    read_and_print_hostname("/etc/hostname", "parent-before");

    printf("[parent] Creating btrfs snapshot at %s\n", SNAP_PATH);
    if (system("btrfs subvolume snapshot / " SNAP_PATH) != 0) {
        perror("btrfs snapshot failed");
        return 1;
    }

    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        return 1;
    }

    if (pid == 0) {
        // Child
        printf("[child] In forked process\n");

        if (unshare(CLONE_NEWNS) < 0) {
            perror("unshare");
            exit(1);
        }

        if (chroot(SNAP_PATH) < 0) {
            perror("chroot");
            exit(1);
        }
        if (chdir("/") < 0) {
            perror("chdir");
            exit(1);
        }

        // Read hostname string before modification
        read_and_print_hostname("/etc/hostname", "child-before");

        // Change hostname and overwrite /etc/hostname
        const char *new_hostname = "snapshot-test";
        if (sethostname(new_hostname, strlen(new_hostname)) < 0) {
            perror("sethostname");
            exit(1);
        }
        printf("[child] Hostname changed via sethostname()\n");

        FILE *f = fopen("/etc/hostname", "w");
        if (!f) {
            perror("fopen");
            exit(1);
        }
        fprintf(f, "%s\n", new_hostname);
        fclose(f);
        printf("[child] /etc/hostname updated inside snapshot.\n");

        // Read back the modified string
        read_and_print_hostname("/etc/hostname", "child-after");

        printf("[child] Exiting now.\n");
        exit(0);
    }

    // Parent waits for child
    waitpid(pid, &status, 0);
    printf("[parent] Child finished with status %d\n", status);

    // Parent: read /etc/hostname after child exits
    read_and_print_hostname("/etc/hostname", "parent-after");

    // Cleanup snapshot
    printf("[parent] Deleting snapshot %s\n", SNAP_PATH);
    char cmd[256];
    snprintf(cmd, sizeof(cmd), "btrfs subvolume delete %s", SNAP_PATH);
    if (system(cmd) != 0) {
        perror("btrfs subvolume delete failed");
    } else {
        printf("[parent] Snapshot deleted successfully\n");
    }

    return 0;
}

