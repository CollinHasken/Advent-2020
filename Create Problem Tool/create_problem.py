#0<0# : ^
''' 
@echo off
echo batch code
python "%~f0" %*
pause
exit /b 0
'''
import sys, getopt, subprocess, os
import fileinput
import xml.etree.ElementTree as ET

def CreateFile(full_path, template_file):
	subprocess.call('copy "' + template_file + '" "' + full_path + '"', shell=True)
    
def Substitute(full_path, replacements):
	with fileinput.FileInput(full_path, inplace=True) as file:
		for line in file:
			new_line = line
			for original, replacement in replacements:
				new_line = new_line.replace(original, replacement)
			print(new_line, end='')
            
def ReplaceInbetween(full_path, replacements):    
    with fileinput.FileInput(full_path, inplace=True) as file:
        for line in file:
            new_line = line
            for start, end, replacement in replacements:
                start_pos = new_line.find(start)
                if start_pos != -1:
                    new_line_without_start = new_line[start_pos + len(start):]
                    end_pos = new_line_without_start.find(end)
                    if(end_pos != -1):
                        new_line = new_line[:start_pos + len(start)] + replacement + new_line[start_pos + len(start) + end_pos:]
            print(new_line, end='')
    
def StringsInString(reqs, orig):
    for req in reqs:
        if req not in orig:
            return False
    return True
    
def PlaceInAlphabetical(file, new_string, find_strings, start_strings = None, lines_spanned = 1, compare_string = None):
	#TODO
	# change to pattern start and end instead of span for first instance test case
	printed_string = False
	if start_strings is None:
		start_strings = find_strings
	if compare_string is None:
		compare_string = new_string
	started = False
	pattern_found = False
	lines = 0
	temp_string = ""
	first_line = ""
	for line in fileinput.FileInput(file, inplace=True):
		if not started and StringsInString(start_strings, line):
			started = True

		if not started:
			print(line, end='')
			continue

		if not pattern_found:
			if not StringsInString(find_strings, line):
				print(line, end='')
				continue
			pattern_found = True
			lines = lines_spanned

		if not printed_string :
			if lines == lines_spanned:
				first_line = line			
			lines =  lines - 1
			temp_string += line
			if lines == 0:
				if StringsInString(find_strings, first_line):
					if compare_string < first_line:
						print(new_string + "\n" + temp_string, end='')
						printed_string = True
					else:
						print(temp_string, end='')
						lines = lines_spanned
						temp_string = ""
				else:
					# This is last alphabetically
					print(new_string + "\n" + temp_string, end='')
					printed_string = True
			continue

		print(line, end='')
        
def main(argv):
    path = ""
    project = ""
    day = ""
    opts, args = getopt.getopt(argv,"p:s:d:",["path=solution=day="])
    for opt, arg in opts:
        if opt in ("-p", "--path"):
            path = arg
        elif opt in ("-s", "--solution"):
            project = arg
        elif opt in ("-d", "--day"):
            day = str(arg)
    print(day, path)
    path = path.replace('\\', '/')
    
    template_file_path = r'H:/Program Files (x86)/Microsoft Visual Studio/2019/Community/Common7/IDE/MyTools/day_template'
    project_file = path + project
    project_filter_file = path + project + '.filters'
    main_cpp = path + 'main.cpp'
    problems_path = path + "problems"
    problems_h = problems_path + r'/problems.h'
    input_file  = r"problems/inputs/input_day_" + day + '.txt'
    full_input_path = path + input_file
    days_path = problems_path + r"/days"    
    day_file_path = days_path + r"/day_" + day
    day_file_h = day_file_path + '.h'
    day_file_cpp = day_file_path + '.cpp'
    
    problem_local_path = "problems\\days"
    problem_day_local_path = problem_local_path + "\\day_"
    
    substitutions = [("TEMPLATE_DAY", str(day))]
    
    CreateFile(day_file_h.replace('/', "\\"), template_file_path.replace('/', "\\") + '.h')
    Substitute(day_file_h, substitutions)
        
    schema = '{http://schemas.microsoft.com/developer/msbuild/2003}'
    ET.register_namespace('', 'http://schemas.microsoft.com/developer/msbuild/2003')    
    project_parser = ET.parse(project_file)
    root = project_parser.getroot()
    for item_group in root.iter(schema + 'ItemGroup'):
        if item_group.attrib != {}:
            continue
        if item_group.find(schema + "ClInclude") is not None:
            element = item_group.makeelement(schema + 'ClInclude', {"Include" : problem_day_local_path + day + ".h"})
            element.tail = '\n\t'
            item_group.append(element)
    project_parser.write(project_file, encoding = "utf-8", xml_declaration=True)    
    project_filter_parser = ET.parse(project_filter_file)
    root = project_filter_parser.getroot()
    for item_group in root.iter(schema + 'ItemGroup'):
        child = item_group.find(schema + "ClInclude")
        if child is None:
            continue
        element = item_group.makeelement(schema + 'ClInclude', {"Include" : problem_day_local_path + day + ".h"})
        element.text = '\n\t\t'
        element.tail = '\n\t'
        filter_element = element.makeelement(schema + 'Filter', {})
        filter_element.text = "Header Files\\" + problem_local_path
        filter_element.tail = '\n\t'
        element.append(filter_element)
        item_group.append(element)
    project_filter_parser.write(project_filter_file, encoding = "utf-8", xml_declaration=True)
    print("day H done")
    
    CreateFile(day_file_cpp.replace('/', '\\'), template_file_path.replace('/', "\\") + '.cpp')
    Substitute(day_file_cpp, substitutions)
    
    project_parser = ET.parse(project_file)
    root = project_parser.getroot()
    for item_group in root.iter(schema + 'ItemGroup'):
        if item_group.attrib != {}:
            continue
        if item_group.find(schema + "ClCompile") is not None:
            element = item_group.makeelement(schema + 'ClCompile', {"Include" : problem_day_local_path + day + ".cpp"})
            element.tail = '\n\t'
            item_group.append(element)
    project_parser.write(project_file, encoding = "utf-8", xml_declaration=True)
    project_filter_parser = ET.parse(project_filter_file)
    root = project_filter_parser.getroot()
    for item_group in root.iter(schema + 'ItemGroup'):
        child = item_group.find(schema + "ClCompile")
        if child is None:
            continue
        element = item_group.makeelement('ClCompile', {"Include" : problem_day_local_path + day + ".cpp"})
        element.text = '\n\t\t'
        element.tail = '\n\t'
        filter_element = element.makeelement('Filter', {})
        filter_element.text = "Source Files\\" + problem_local_path
        filter_element.tail = '\n\t'
        element.append(filter_element)
        item_group.append(element)
    project_filter_parser.write(project_filter_file, encoding = "utf-8", xml_declaration=True)
    print("day cpp done")
    
    PlaceInAlphabetical(problems_h, r'#include "days/day_' + str(day) + '.h"', (r'#include'))
    print("problem include done")
    
    replacements = [("PROBLEM_CLASSES_INSTANTIATE_AND_SOLVE(", ", ", str(day)), (r', "', r'");', input_file]
    ReplaceInbetween(main_cpp, replacements)
    print("main done")
    
    CreateFile(full_input_path.replace('/', "\\"), 'NUL')
    print("input done")
    
    os.startfile(full_input_path)
    
main(sys.argv[1:])