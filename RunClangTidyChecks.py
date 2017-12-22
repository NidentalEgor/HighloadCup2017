import os
import argparse
import subprocess

parser = argparse.ArgumentParser(description='Run some clang-tidy checks on all c++ files.')
parser.add_argument(
    'output_directory_path',
    metavar='out',
    type=str,
    nargs=1,
    help='path to output directory')

excluded_directories = ['Build', 'build', '.git', 'Submodules', '.vscode']
current_directory = os.path.dirname(os.path.abspath(__file__))

for root, directories, filenames in os.walk(os.path.dirname(os.path.realpath(__file__))):
    directories[:] = [d for d in directories if d not in excluded_directories]
    filenames[:] = [f for f in filenames if (os.path.splitext(f)[1] == '.cpp')]

    for filename in filenames:
        output_file_path = os.path.join(
            current_directory + "/Build/ClangTidyLogs",
            os.path.splitext(filename)[0] + "_clang_tidy.logs")

        bash_command =\
        "clang-tidy -checks=* -header-filter=*.unexisting -p " + current_directory +\
        "/Build/Linux/Release/compile_commands.json " + os.path.join(root, filename)

        process = subprocess.Popen(bash_command.split(), stdout=subprocess.PIPE)
        output, error = process.communicate()
        
        logs = str(output)
        if "Submodules" not in logs:
            print(logs.replace('\\n', '\n'))
