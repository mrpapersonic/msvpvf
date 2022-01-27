import sys, argparse, os

def prog_type(byte):
    if byte == 0xF6:
        return "Movie Studio"
    elif byte == 0xEF:
        return "VEGAS Pro"
    else:
        return "Unknown"

def type_hex(type):
    if type == "vf" or type == 1:
        return "MS", [0xF6, 0x1B, 0x3C, 0x53, 0x35, 0xD6, 0xF3, 0x43, 0x8A, 0x90, 0x64, 0xB8, 0x87, 0x23, 0x1F, 0x7F]
    elif type == "veg" or type == 0:
        return "PRO", [0xEF, 0x29, 0xC4, 0x46, 0x4A, 0x90, 0xD2, 0x11, 0x87, 0x22, 0x00, 0xC0, 0x4F, 0x8E, 0xDB, 0x8A]
    else:
        return

def main(args):
    if args.input:
        inputfile = args.input
    if os.path.exists(inputfile):
        with open(inputfile, "rb") as fp:
            test = bytearray(fp.read())
    else:
        print("Input file doesn't exist!")
        exit()
    project = prog_type(test[0x18])
    print("Project file version: " + project + " " + str(test[0x46]) + ".0")
    if not args.version:
        answer = ""
    else:
        answer = args.version
    while answer not in ["8", "9", "10", "11", "12", "13", "14", "15", "16", "17", "18"]:
        answer = str(input("Which version of VEGAS would you like it to be spoofed to? [8-18]: "))
    if not args.type:
        answer2 = ""
        while answer2 not in ["veg", "vf"]:
            answer2 = input("Would you like it to be VEGAS Pro or Movie Studio? [veg,vf]: ").lower()
    else:
        answer2 = args.type
    filename_prefix, typehex = type_hex(answer2)
    test[0x18:0x27] = typehex
    test[0x46] = int(answer)
    filename_wo_ext = os.path.basename(os.path.splitext(inputfile)[0])
    if args.output:
        outputfile = args.output
    else:
        outputfile = os.path.abspath(os.path.dirname(inputfile)) + "/" + filename_prefix + "_V" + answer + "_" + str(filename_wo_ext) + "." + answer2
    with open(outputfile, 'wb') as target:
    	target.write(test)
if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument('-i', '--input', help="input file", metavar='<input>', required=True)
    parser.add_argument('-o', '--output', help="output file, defaults to '(MS,VEG)_(version)_(original name).(vf,veg)'", metavar='<output>')
    parser.add_argument('-v', '--version', help="output file version", metavar='<version>')
    parser.add_argument('-t', '--type', help="output file type, vf/veg", metavar='<type>')
    args = parser.parse_args()
    main(args)
