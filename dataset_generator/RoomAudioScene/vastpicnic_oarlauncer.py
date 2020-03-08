#!/usr/bin/env python
# -*- coding: utf-8 -*-

# ALL DIRECTORY NAMES SHOULD END WITH '/'
RESULTS_DIR = "./Results/"    # On the grid
LOG_DIR = "./Logs/"            # On the grid
SEND_RESULTS_PATH = "~/Documents/Code/" # Laptop
SEND_RESULTS_DIR = "./Results_IGRIDA/"  # Except here: no trailing / here

MATLAB_FUN = "main" # script that launch matlab binary
UNAME = "ddicarlo"            # To check your jobs with oarstat
LAPTOP = "embuscade"

MATLAB_ROOT = "/soft/matlab_hd/R2017a/" # path to matlab on IGRIDA

###############################################################################
## HYPERPARAMETERS DEFINITIONS OF THE MATLAB FUNCTION
###############################################################################

# debug version: if true do not submit any job
DEBUG = False;
# name for the job id: three capital letters
shortname = 'VST'
# parameters of the matlab function
final_dataset_size = 100e3 # number of entries in the final dataset (after merged)
n_task = 0                 # [init] a parameter configuration
n_jobs_per_task = 400      # number a of job for the same configuration
dataset_size_per_job = int(final_dataset_size/n_jobs_per_task);

# PARAMETER SPACE:
# Set of SORTED input parameters for each job !!! ATTENTION: here order matters
PRMS_NAMES = ["n_rirs", "filenames"]
# Dictionary that define the ranges of the parameters
# !!! ATTENTION each value must a LIST!
parameters_and_ranges = {
    "n_rirs" :  [dataset_size_per_job],
    "filenames" : ['task0_fixRoom'],
}



###############################################################################
## FUNCTION DEPENDING ON THE PARAMS CONFIGURATION
###############################################################################

# If you have param configuration to prune out from the parameter space
# you should write some condition here
def condition_to_prune_the_param_space(param_dict):
    return True
    # if param_dict["do_realistic_room"] > 0      \
    #     and param_dict["other_walls_abs"] > 0   \
    #     and param_dict["south_wall_abs"] > 0:
    #     return True
    # return False

# Compute number of core and walltime duration accoding to some params
def compute_resources(params_dict):
    # p = scan_params_from_string(params_string)
    gb_per_100rirs = 0.015 # 15 MB for 100 rirs
    hr_per_100rirs = 1     # 1 hour for for 100 rirs
    mem_gb = (params_dict['n_rirs']/100*gb_per_100rirs*1.5)
    tim_hr = (params_dict['n_rirs']/100*hr_per_100rirs*1.5)
    # This is a rough approximation, the memory/core depends on the cluster!
    if mem_gb > 6:
        nb_cores = 12
    elif mem_gb > 4:
        nb_cores = 8
    elif mem_gb > 2:
        nb_cores = 4
    else:
        nb_cores = 2
    max_duration_hours = int(round(tim_hr)) + 1
    return (nb_cores, max_duration_hours)

### YOU SHOULD NOT MODIFY BELOW HERE ###
# path to the bash wrapper file (including the matlab root argument).
# this bash file (run_XXXXXX.sh) Sets up the MATLAB Runtime environment
# for the current $ARCH and executes the specified command.
BIN = "./bin/run_" + MATLAB_FUN + ".sh " + MATLAB_ROOT + " "
# path to the the file in the yuor local session
PATH_SEND_RESULTS = UNAME + "@" + LAPTOP + ":" + SEND_RESULTS_PATH+SEND_RESULTS_DIR

###############################################################################
## IMPORTS
###############################################################################

import os
import subprocess
from itertools import product
import signal
# !!! ATTENTION on igrida you it is better not to have a virtualenv,
# so do avoid to use non standard imports

###############################################################################
## SOME AUXILIARY FUNCTIONS
###############################################################################

# usefull class that allows take actions after a timeout
class timeout:
    def __init__(self, seconds=1, error_message='Timeout'):
        self.seconds = seconds
        self.error_message = error_message
    def handle_timeout(self, signum, frame):
        raise TimeoutError(self.error_message)
    def __enter__(self):
        signal.signal(signal.SIGALRM, self.handle_timeout)
        signal.alarm(self.seconds)
    def __exit__(self, type, value, traceback):
        signal.alarm(0)

# Executes a shell command and returns the output without '\n' at the end.
def execute(c):
    return subprocess.check_output(c, shell=True).rstrip()

# Asks a y/n question, with No as default.
def ask_binary_question(q):
    answer = raw_input(q+" [y/N] ").lower()
    return (answer == 'yes' or answer == 'y')

# Generate the parameter space:
# for a dictionary of list, compute all the possible combinations
# modify the function condition_to_prune_the_param_space to avoid
# some particoular combination
def gen_all_combinations(**args):
    tupargs = list(args.items())
    keys = list(map(lambda x:x[0],tupargs))
    for item in product(*(args[prm] for prm in PRMS_NAMES)):
        tmp = dict(zip(keys,item))
        if condition_to_prune_the_param_space(tmp):
            yield tmp

# # Generate a short version of the filename with only the values
# def short_name(oldstr):
#     for char in PRMS_NAMES:
#         oldstr.replace(char, "")
#     return oldstr.replace("-", "")

# def parse_short_name(short_name):
#     params_str = short_name.split('_')
#     params_dict = {}
#     for idx, value in enumerate(params_str):
#         params_dict[PRMS_NAMES[idx]] = value
#     return params_dict

def parse_full_name(full_name):
    return

# Extract the parameter from the string
def scan_params_from_string(params_string):
    p = parse(PRMS_FRMT, params_string)
    return p

# For each point in the parameters space generate the suffix name
def from_params_dict_to_tuple(item):
    return tuple([item[k] for k in PRMS_NAMES])

# Returns the number of result lines in [fn], or 0 if the file does not exist.
def get_lines(fn):
    if os.path.isfile(fn):
        cmd = "wc -l %s | cut -f 1 -d ' '" % fn
        nb_lines = int(execute(cmd))
        if nb_lines>0:
            nb_lines = nb_lines-1   # Do not count the header line
    else:
        nb_lines = 0
    return nb_lines

def ask_send_results(q="Should I send the results to your laptop"):
    send = ask_binary_question(q+" (path is '"+str(PATH_SEND_RESULTS)+"') ?")
    if send:
        # print "Well, this is actually not implemented yet. The programmer must have been lazy."
        rsync_cmd = "rsync -avz -e 'ssh' "+RESULTS_DIR+" "+PATH_SEND_RESULTS
        execute(rsync_cmd)

# Generates the job "wrapper" command, i.e. including 'oarsub'
def gen_wrapper_command(cmd, shortname, suffix, nb_cores, max_duration_hours):
    # Generate outputs
    outn = LOG_DIR + shortname + "_%jobid%.out"
    errn = LOG_DIR + shortname + "_%jobid%.err"
    # Generate wrappper command
    cmd = "oarsub -l "                       \
            + "/nodes=1"                      \
            + "/core="    + str(nb_cores) + "," \
            + "walltime=" + str(max_duration_hours) + ":00:00 "  \
            + "-S \""     + cmd  + " " + suffix + "\" " \
            + "-n " + shortname  + " "       \
            + "-O " + outn + " "             \
            + "-E " + errn
    return cmd

def get_jobs():
    cmd = "oarstat -u "+UNAME+" -f | grep command | cut -d' ' -f7-"
    running = execute(cmd)
    # running is the list of commands, 1 command per line
    return running

# Returns if [pcmd] is already running or waiting to be launched.
def check_job(pcmd, running):
    return pcmd in running

def res_oarsub(res):
    return ("Generate a job key" in res)

# Returns the number of result lines in [fn], or 0 if the file does not exist.
def get_lines(fn):
    if os.path.isfile(fn):
        cmd = "wc -l %s | cut -f 1 -d ' '" % fn
        nb_lines = int(ex(cmd))
        if nb_lines>0:
            nb_lines = nb_lines-1   # Do not count the header line
    else:
        nb_lines = 0
    return nb_lines

# Makes a first loop on the parameter space to compute the number of jobs,
# and check if some results have already been computed.
# def collect_data(submitted_commands):
#     nb_total      = 0
#     nb_missing    = 0
#     nb_incomplete = 0
#     nb_planned    = 0 # A planned job can be "missing" or "incomplete" as well!
#     nbl_total     = 0
#     nbl_computed  = 0

#     param_dicts_list = gen_list_of_params_dict(parameters_and_ranges)

#     for param_dict in param_dicts_list:
#         # print sn
#         nb_total = nb_total + 1
#         nbl_total = nbl_total + its
#         nb_lines = get_lines(fn)
#         nbl_computed = nbl_computed + nb_lines
#         if nb_lines == 0:
#             nb_missing = nb_missing + 1
#         if nb_lines > 0 and nb_lines < its:
#             # print "nb_lines = "+str(nb_lines)+"/"+str(its)
#             nb_incomplete = nb_incomplete + 1
#         params_str = params2string(params_dict)
#         planned = check_job(params_str, submitted_commands)
#         if planned:
#             nb_planned = nb_planned + 1
#     p_missing    = int(nb_missing / float(nb_total) * 100)
#     p_incomplete = int(nb_incomplete / float(nb_total) * 100)
#     pl_missing   = int((nbl_total - nbl_computed) / float(nbl_total) * 100)
#     p_planned    = int(nb_planned / float(nb_missing + nb_incomplete) * 100)
#     nb_relaunch  = nb_incomplete + nb_missing - nb_planned       # Relaunch if missing
#     return (nb_total, nb_missing, nb_incomplete, nb_relaunch, p_missing, p_incomplete, pl_missing, p_planned)


def print_infos(nb_total, p_missing, p_incomplete, p_planned, pl_missing):
    print "Total number of jobs: "+str(nb_total)+"."
    print "Missing files: "+str(p_missing)+"%."
    print "Incomplete files: "+str(p_incomplete)+"%."
    print "From these missing or incomplete, "+str(p_planned)+"% are already running or planned."
    print "Missing lines: "+str(pl_missing)+"%."

def print_dict(d):
    for k in d.keys():
        print(k, d[k])

# compose the string of argument for the binary file from the dictionary
# according to the values in the list PRMS_NAMES
def params2string(d):
    s = "";
    print(d)
    for p in PRMS_NAMES:
        s = s + str(d[p]) + " "
    return s

class bcolors:
    HEADER = '\033[95m'
    OKBLUE = '\033[94m'
    OKGREEN = '\033[92m'
    WARNING = '\033[93m'
    FAIL = '\033[91m'
    ENDC = '\033[0m'
    BOLD = '\033[1m'
    UNDERLINE = '\033[4m'

def gen_list_of_params_dict(param_space_dict):
    if len(param_space_dict.keys()) == 1:
        return [param_space_dict] # create a singleton list
    if len(param_space_dict.keys()) == 0:
        print("No parameters space provide... woooow see ya!")
        return exit(0)
    # generate list of all comb of parameters form the given dict
    return list(gen_all_combinations(**param_space_dict))

def oar_submit(wcmd):
    try:
        # let s wait 4 seconds before getting crazy...
        with timeout(seconds=4):
            print("\t" + bcolors.BOLD + wcmd + bcolors.ENDC)
            if not DEBUG:
                execute(wcmd)
            print("  submitted!")
            return 0
    except:
        print(bcolors.WARNING \
            + "MEEGA FAIL!!\n" + bcolors.ENDC \
            + "\tNo worries, we will try it in a while..."\
            )
        return oar_submit(wcmd)

###############################################################################
### HERE COMES THE MAGIC
###############################################################################

def main():

    # submitted_commands = get_jobs()
    # (nb_total, nb_missing, nb_incomplete, nb_relaunch, \
    #     p_missing, p_incomplete, pl_missing, p_planned) \
    #         = collect_data(submitted_commands)
    # 1/0
    print(bcolors.OKGREEN
        + "\n==================================================\n"  \
        +  "=                    w|-|eLLc0me                 =\n"   \
        +  "=               please help yourself             =\n"   \
        +  "==================================================\n\n" \
        + bcolors.ENDC)
    print("The magic trick of today is: \n\trunning " \
          + bcolors.OKGREEN + MATLAB_FUN + bcolors.ENDC + "\n")

    # Create directories if necessary
    execute("mkdir -p "+ RESULTS_DIR)
    execute("mkdir -p "+ LOG_DIR)

    if False:
        print("Not yet implemented.")
    # if nb_relaunch>0:
    #     currently_running = 0
        # if nb_relaunch == nb_total:
        #     print "Well well well, looks like there are no results yet."
        #     b = ask_binary_question("You're about to launch "+str(nb_total)+" scripts. Proceed?")
        #     if not b:
        #         exit(0)
        # else:
        #     print_infos(nb_total, p_missing, p_incomplete, p_planned, pl_missing)
        #     print "------------------------------"
        #     b = ask_binary_question("You're about to relaunch "+str(nb_relaunch)+" scripts. Proceed?")
        #     if not b:
        #         exit(0)
        # # This is where we actually submit the jobs
        # for (p, sn, fn, cmd, pcmd, its) in gen_params():
        #     nbl = get_lines(fn)
        #     nbl_to_compute = its - nbl
        #     planned = check_job(pcmd, submitted_commands)
        #     if nbl_to_compute>0 and not planned: # then we actually need to do something…
        #         (nb_cores, max_duration_hours) = compute_ressources(p)
        #         wcmd = gen_wrapper_command(cmd, sn, nb_cores, max_duration_hours)
        #         # print "'"+wcmd+"' must run."
        #         res = ex(wcmd)
        #         if not res_oarsub(res):
        #             print res
        #         else:
        #             print "[OK] "+sn
        # if nb_relaunch<nb_total:
        #     ask_send_results("Should I send intermediate results to your laptop")
    else:
        print("So, you want to run some experiments, aren't you?")

        param_dicts_list = gen_list_of_params_dict(parameters_and_ranges)

        # print how many jobs it is going to be launched
        n_task = len(param_dicts_list)
        n_jobs = n_jobs_per_task * n_task
        print(("You are going to submit %s %g jobs %s to IGRIDA:\n"\
               + "\t%g jobs x %g tasks\n")
            %(bcolors.OKGREEN, n_jobs, bcolors.ENDC, n_jobs_per_task, n_task))

        # ask for confirmation
        if not ask_binary_question("Wanna do it?"):
            print('Ahhh, it was nice. See ya later!')
            return
        print("\n")

        # job counter
        jobcount = 1;
        # if so, submit jobs for all parameters conbination
        for i, params_dict in enumerate(param_dicts_list):

            print("Considering tasks %s%d/%d%s:\n"\
                %(bcolors.OKGREEN,i+1,n_task,bcolors.ENDC))

            # n_job for each parameter configuration
            for j in range(n_jobs):

                if j > n_jobs -1:
                    print("  Submitting job %s%d/%d%s:"\
                        %(bcolors.OKGREEN,j+1,n_jobs,bcolors.ENDC))
                else:
                    print("  Submitting job %s%d/%d%s:"\
                        %(bcolors.OKGREEN,j+1,n_jobs,bcolors.ENDC))

                #from param dict to string for the matlab binary
                params_str = params2string(params_dict)
                #compute resources (n_core, time) for the job
                n_cores, max_duration_hours = compute_resources(params_dict)
                wcmd = gen_wrapper_command(
                                BIN,                         # path to the binar
                                shortname + str(jobcount),   # shortname for JOBID
                                params_str,                  # string of parameters
                                n_cores, max_duration_hours) # resources
                oar_submit(wcmd)

                jobcount += 1

                print("\n")
            print("\n")
        print(" SO LONG AND THANKS FOR THE FISH.")
    # end main

if __name__ == '__main__':
    main()
