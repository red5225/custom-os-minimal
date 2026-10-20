# How to make changes safely

Use this routine for every change. It is the software equivalent of saving your work before experimenting.

## 1. Save a known-good point

On GitHub, make a commit before changing code. Give it a clear message such as `docs: explain the shell`. If the change breaks the build, you can compare commits or revert the commit.

## 2. Change one small thing

Good first edits:

- Change the text printed by `about` in `kernel/shell.c`.
- Add a comment explaining a function.
- Add one command using the instructions in `ADD_FEATURES.md`.

Avoid changing several files at once until you understand how they connect.

## 3. Build and test

Run `make clean` then `make iso`, or push your change and watch GitHub Actions. Read the first compiler or assembler error, not only the final failure summary. Never assume the ISO exists unless the build step succeeded.

## 4. Test in a virtual machine

Boot the ISO in QEMU. Check that startup messages appear and try the feature you changed. If the screen is blank, use serial logs before changing random settings.

## 5. Review the change

Ask yourself:

- Did I change only what I meant to change?
- Did I preserve bounds checks and error checks?
- Did I update documentation?
- Did the build and QEMU smoke test pass?

## 6. Undo a bad change

The safest option for a beginner is to use GitHub's commit history and revert the specific bad commit, or edit the file back to its previous version. Do not force-push or delete the whole repository to undo one mistake.

## Rules for low-level code

- Do not copy random register values into hardware ports.
- Do not write to arbitrary physical memory addresses.
- Do not edit page tables, linker addresses, CPU mode transitions, or interrupt code without a specific reason and a QEMU test.
- Never use your internal drive as a test target for the ISO.
- A kernel crash in a VM is recoverable; a firmware or disk mistake on real hardware may not be.
