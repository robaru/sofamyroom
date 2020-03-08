from __future__ import print_function

import time
import os
import subprocess
from itertools import product


# matlab function to be compiled (must end with .m)
MATLAB_FUN_TO_COMPILE = "./mFiles/main.m"
additional_filename = None

DEBUG = False

OUTPUT_DIR = "./bin/"
LOG_DIR = "./Logs/"
FILES_DIR = "./mFiles/"
MATLAB_VERSION = "R2017a"
PATH_TO_MATLAB = "/soft/matlab_hd/" + MATLAB_VERSION + "/"
PATH_TO_MATLAB_COMPILER = PATH_TO_MATLAB + "bin/mcc"

PATH_TO_SCHIMMEL = "./mFiles/roomsim.mexa64"

print("Output compilation dir: %s"%(OUTPUT_DIR))
print("File to compile: %s"%(MATLAB_FUN_TO_COMPILE))
print("")

def gen_compilation_command():
	if not additional_filename:
		cmd = PATH_TO_MATLAB_COMPILER \
				+ " -mv " + MATLAB_FUN_TO_COMPILE \
				+ " -d " + OUTPUT_DIR \
				+ " -R -singleCompThread"
	else:
		cmd = PATH_TO_MATLAB_COMPILER \
				+ " -mv " + MATLAB_FUN_TO_COMPILE \
				+ " -d " + OUTPUT_DIR \
				+ " -a "
		# add additional files
		for arg in additional_filename:
			cmd += " " + arg
		cmd += " -R -singleCompThread"
	return cmd

def write_bash_file(command, filename):
	text = \
		  "#Load some modules"\
		+ "source /etc/profile.d/modules.sh 			\n"\
		+ "												\n"\
		+ "#Setup MCR cache directory locally			\n"\
		+ "export MCR_CACHE_ROOT=/tmp/mcr_cache_${USER}_OAR_JOBID_${OAR_JOBID} \n"\
		+ "mkdir -p $MCR_CACHE_ROOT 					\n"\
		+ "# Run to compile 							\n"\
		+ command + " \n" \
		+ "												\n"\
		+ "#Remove temporary MCR cache directory 		\n"\
		+ "/bin/rm -rf $MCR_CACHE_ROOT"

	print("Generating the makefile... ")
	# remove older version if exist
	execute("rm -f " + filename)
	with open(filename, "a") as f:
		f.write(text)
	# make the file executable
	execute("chmod +x " + filename)
	print("done")
	return filename

# Executes a shell command and returns the output without '\n' at the end.
def execute(c):
	if DEBUG:
		print(c)
		return 1
	return subprocess.check_output(c, shell=True).rstrip()

# Generates the job "wrapper" command, i.e. including 'oarsub'
def gen_wrapper_command(path_to_script, shortname, suffix = "", nb_cores=2, max_duration_hours=1):
    # Generate outputs
    outn = LOG_DIR + shortname + "_%jobid%.out"
    errn = LOG_DIR + shortname + "_%jobid%.err"
    # Generate wrapper command
    cmd = "oarsub -l "                        \
            + "/nodes=1"                      \
            + "/core="    + str(nb_cores)   + "," \
            + "walltime=" + str(max_duration_hours) + ":00:00 "      \
            + "-S \""     + path_to_script  + " "   + suffix + "\" " \
            + "-n " + shortname  + " "       \
            + "-O " + outn + " "             \
            + "-E " + errn
    return cmd

def get_job_id(s):
	last_word = s.rsplit(None, 1)[-1]
	return last_word.replace("OAR_JOB_ID=", "")

def main():
	print("You are about to run the compilation the file: ",
			MATLAB_FUN_TO_COMPILE)
	# create the output folder
	execute("mkdir -p " + OUTPUT_DIR)
	# create a folder for temporary files
	tmp_dir = "./tmp/"
	execute("mkdir -p " + tmp_dir)
	# generate the compilation command for mcc
	compilation_cmd = gen_compilation_command()
	# write the bash file wrapper used by oarsub
	makefile = "./makefile.sh"
	oarcmd = gen_wrapper_command(
				write_bash_file(compilation_cmd, makefile),
				"MAKU", "")

	# when executing a OARSUB command
	# it returns a string containing the ID of the submitted job
	out = execute(oarcmd)
	jobid = get_job_id(out)
	print("Job submitted: %s\n"%(jobid))
	oarcmd_is_running = True

	print("I am waiting for the compilation to end...")
	twait = 100 # [seconds]
	while out:
		out = int(execute("oarstat -u ddicarlo " \
						+ " | grep " + jobid \
						+ " | wc -l" ))
		time.sleep(twait)
		print("... still waiting ...")

	print("Compilation finished.\n")

	print("Cleaning up...")
	execute("rm -rf " + tmp_dir)
	execute("rm -f "  + makefile)
	# remove some useless compilation file
	#execute("rm " + OUTPUT_DIR + "*.log")
	#execute("rm " + OUTPUT_DIR + "*.txt")
	# copy Schimmel's binary
	execute("cp ./mFiles/roomsim.mexa64 ./bin/")
	print("done.")

if __name__ == '__main__':
	main()
