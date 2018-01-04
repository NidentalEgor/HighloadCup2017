import os
import os.path as osp
import shutil
import subprocess
# import argparse

# parser = argparse.ArgumentParser(description='Run some clang-tidy checks on all c++ files.')
# parser.add_argument(
#     'output_directory_path',
#     metavar='out',
#     type=str,
#     nargs=1,
#     help='path to output directory')

excluded_directories = ['Build', 'build', '.git', 'Submodules', '.vscode']
current_directory = osp.dirname(osp.dirname(osp.abspath(__file__)))

logs_folder = osp.join(current_directory, "Build/ClangTidyLogs")
if os.path.exists(logs_folder):
    shutil.rmtree(logs_folder)
os.makedirs(logs_folder)

for root, directories, filenames in os.walk(osp.dirname(osp.dirname(osp.realpath(__file__)))):
    directories[:] = [d for d in directories if d not in excluded_directories]
    filenames[:] = [f for f in filenames if (osp.splitext(f)[1] == '.cpp')]

    for filename in filenames:
        output_file_path = osp.join(
            logs_folder,
            osp.splitext(filename)[0] + "_clang_tidy.logs")

        bash_command =\
        "clang-tidy -checks=* -header-filter=*.unexisting -p " + current_directory +\
        "/Build/Linux/Release/compile_commands.json " + osp.join(root, filename)

        process = subprocess.Popen(bash_command.split(), stdout=subprocess.PIPE)
        output, error = process.communicate()
        
        logs = str(output)
        log_file = open(output_file_path, 'w')
        if "Submodules" not in logs:
            log_file.write(logs.replace('\\n', '\n'))
