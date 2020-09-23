import re
import sys
import getopt
import os


def usage():
    print("""
Usage: command [option] <java filename>
 * -c --class for output list of class names
 * -f --file for output list of class output filenames.
 * -h --header for output list of jni header output filenames.
Notice: only print public class for now version
""")


if __name__ == '__main__':
    if len(sys.argv) < 2:
        usage()
        exit(1)

    OPTION_CLASS = 1
    OPTION_FILE = 2
    OPTION_HEADER = 3

    opts = []
    args = list(sys.argv[1:])
    try:
        opts, args = getopt.getopt(args, "-c-f-h", ["class", "file", "header"])
    except getopt.GetoptError as e:
        sys.stderr.write("{}".format(e))
        exit(2)

    if len(args) != 1:
        usage()
        exit(3)
    setup = set([opt[0] for opt in opts])
    if len(setup) > 1:
        sys.stderr.write("Only one option can be set, got: {}\n".format(list(setup)))
        exit(5)
    output_option = OPTION_HEADER
    if len(setup.intersection({"-c", "--class"})) > 0:
        output_option = OPTION_CLASS
    elif len(setup.intersection({"-f", "--file"})) > 0:
        output_option = OPTION_FILE
    elif len(setup.intersection({"-h", "--header"})) > 0:
        output_option = OPTION_HEADER

    java_filename = args[0]
    content = ""
    try:
        with open(java_filename, "r") as f:
            content = f.read()
    except FileNotFoundError:
        sys.stderr.write("Can not access file: {}\n".format(java_filename))
        exit(4)

    filename = os.path.splitext(os.path.split(java_filename)[-1])[0]

    regex_package = re.compile(r"package\s+([_A-za-z][\._A-za-z0-9]+)\s*;")
    regex_public_class = re.compile(r"[^\{]*class\s+([_A-za-z][_A-za-z0-9]+)[\s\}]+")
    regex_native = re.compile(r"(^|\s)+native\s+")\

    all_package = re.findall(regex_package, content)
    all_public_class = re.findall(regex_public_class, content)
    all_native = re.findall(regex_native, content)

    all_package = list(map(str, all_package))
    all_public_class = list(map(str, all_public_class))
    all_native = list(map(str, all_native))

    if len(all_package) > 1:
        sys.stderr.write("Find duplicate package names: {}\n".format(all_package))
        exit(6)

    if filename not in all_public_class:
        # no public class found
        exit(0)

    if len(all_package) > 0:
        package = all_package[0]
        # check if is an suitable package name
        regex_name = re.compile(r"[_A-za-z][_A-za-z0-9]+")
        for p in package.split("."):
            if not re.match(regex_name, p):
                sys.stderr.write("Got illegal package name: {}\n".format(package))
                exit(7)

    name_class = ".".join(all_package + [filename])

    if output_option == OPTION_CLASS:
        sys.stdout.write(name_class)
        exit(0)
    elif output_option == OPTION_FILE:
        name_file = name_class.replace(".", "/") + ".class"
        sys.stdout.write(name_file)
        exit(0)
    elif output_option == OPTION_HEADER:
        if len(all_native) == 0:
            exit(0)
        name_header = name_class.replace(".", "_") + ".h"
        sys.stdout.write(name_header)
        exit(0)

    exit(0)