#!/usr/bin/python3
import os
import subprocess
import signal
import sys

project_root_dir = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
build_dir = os.path.join(project_root_dir, "build")
config_dir = os.path.join(project_root_dir, "_config")
bats_dir = os.path.join("/bats-protocol-workdir/", "batspro2")
netsim_dir = os.path.join(bats_dir, "Utilities", "simulation")
log_dir = os.path.join(project_root_dir, "_log")
lamp_procs = [None for i in range(4)]


def build():
    print("========================================\n"
          "=================BUILD==================\n"
          "========================================")
    print("Building binary executables...")
    if os.path.exists(build_dir):
        print("\t- Build dir: %s already exists, clean before build." % build_dir)
        os.system("rm -r %s" % build_dir)
    print("\t- Make build directory at: %s" % build_dir)
    os.makedirs(build_dir)

    subprocess.run(["cmake", ".."], cwd=build_dir, check=True)
    subprocess.run(["make", "LamppostHost", "-j4"], cwd=build_dir, check=True)


def deploy():
    print("\n\n\n========================================\n"
          "=================DEPLOY=================\n"
          "========================================")
    print("Start network simulation...")
    with open(os.path.join(log_dir, "BMSimLog.txt"), 'w') as f:
        subprocess.run(["sh", os.path.join(netsim_dir, "bmsim_ipv4.sh"), "start", "4"],
                       cwd=netsim_dir,
                       check=True,
                       stdout=f)
        f.close()

    print("Launch Lamppost host programs...")
    log_lamps = [os.path.join(log_dir, "LamppostHostLog0.txt"),
                 os.path.join(log_dir, "LamppostHostLog1.txt"),
                 os.path.join(log_dir, "LamppostHostLog2.txt"),
                 os.path.join(log_dir, "LamppostHostLog3.txt")]
    for lamp_id in range(4):
        lamp_procs[lamp_id] = subprocess.Popen([os.path.join(build_dir, "LamppostHost"),
                                                "--mock_detection",
                                                "--config_file",
                                                os.path.join(config_dir, "lamp{}.ini".format(lamp_id))],
                                               stdout=log_lamps[lamp_id],
                                               stderr=subprocess.PIPE)


def clean():
    print("\n\n\n========================================\n"
          "==================EXIT==================\n"
          "========================================")
    for lamp_id in range(4):
        if lamp_procs[lamp_id]:
            lamp_procs[lamp_id].kill()


def sig_int_handler(sig, frame):
    print("Ctrl+C or SIGINT is caught!")
    clean()
    sys.exit(0)


def main():
    signal.signal(signal.SIGINT, sig_int_handler)
    print("Log file directory: %s" % log_dir)
    if not os.path.isdir(log_dir):
        os.makedirs(log_dir)

    build()
    deploy()
    signal.pause()


if __name__ == '__main__':
    main()
