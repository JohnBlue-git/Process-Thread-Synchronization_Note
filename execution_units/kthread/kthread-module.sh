#!/bin/sh
set -eu

SCRIPT_DIR=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
KDIR=${KDIR:-/lib/modules/$(uname -r)/build}

usage()
{
    echo "Usage: $0 {build|load|unload|status} MODULE" >&2
    echo "MODULE: basic_kthread mutex semaphore spinlock rwsem seqlock atomic" >&2
    exit 2
}

if [ "$#" -ne 2 ]; then
    usage
fi

action=$1
module=$2

case "$module" in
    basic_kthread|mutex|semaphore|spinlock|rwsem|seqlock|atomic) ;;
    *) usage ;;
esac

is_loaded()
{
    grep -q "^${module} " /proc/modules
}

case "$action" in
    build)
        make -C "$SCRIPT_DIR" "KDIR=$KDIR" "MODULE=$module"
        ;;
    load)
        if [ "$(id -u)" -ne 0 ]; then
            echo "Loading a kernel module requires root; run this action with sudo." >&2
            exit 1
        fi
        if [ ! -f "$SCRIPT_DIR/$module.ko" ]; then
            echo "Module not built. Run: $0 build $module" >&2
            exit 1
        fi
        if is_loaded; then
            echo "$module is already loaded." >&2
            exit 1
        fi
        insmod "$SCRIPT_DIR/$module.ko"
        echo "Loaded $module"
        ;;
    unload)
        if [ "$(id -u)" -ne 0 ]; then
            echo "Unloading a kernel module requires root; run this action with sudo." >&2
            exit 1
        fi
        if ! is_loaded; then
            echo "$module is not loaded." >&2
            exit 1
        fi
        rmmod "$module"
        echo "Unloaded $module"
        ;;
    status)
        if is_loaded; then
            echo "$module is loaded"
        else
            echo "$module is not loaded"
        fi
        ;;
    *)
        usage
        ;;
esac
