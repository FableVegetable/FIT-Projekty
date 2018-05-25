from xml.etree import ElementTree as ET
from optparse import OptionParser
import re, sys, inspect, getopt
import argparse
import os


def exit_with_error( errcode, message):
  """Exits program with specific error code and message.

  Args:
      errcode:  Number that represents error return code of interpret.
      message:  Error info message.

  Returns:
      None

  Usage:
      exit_with_error(errcode_syntax, "Error syntax")
  """
  print ("Error("+str(errcode)+"):  "+message, file=sys.stderr)
  sys.exit(errcode)


#Defined error codes
errcode_params          = 10
errcode_inputfile       = 11
errcode_outputfile      = 12
errcode_internal        = 99
errcode_xml_format      = 31
errcode_syntax          = 32
errcode_semantics       = 52
errcode_operands        = 53
errcode_nonexist_var    = 54
errcode_nonexist_frame  = 55
errcode_missing_value   = 56
errcode_div_by_zero     = 57
errcode_string          = 58

#Filename
filename=""
parser = OptionParser()
parser.add_option("--source", dest="filename",
                help="write report to FILE", metavar="FILE")

#Parsing input arguments
try:
  (options, args) = parser.parse_args()
  if len(sys.argv) !=2:
    raise SystemExit
  option_dict = vars(options)
  filename=option_dict['filename']

except SystemExit:
  if len(sys.argv) !=2:
    exit_with_error(errcode_params, "Input arguments for script 'interpret.py' are incorrect.")
  var = sys.argv[1] 
  if var == "--help" or var == "-h":
    exit(0)
  else:
    exit_with_error(errcode_params, "Input arguments for script 'interpret.py' are incorrect.")

#If file doesnt exist or no access
if not (os.path.isfile(filename) and os.access(filename, os.R_OK) and os.path.exists(filename)):
  exit_with_error(errcode_inputfile, "Input file cannot be opened.")

#Get XML format into tree structure 
try:
  tree = ET.parse(filename).getroot()
except Exception:
  exit_with_error(errcode_xml_format, "XML format is incorrect.")


#Regex patterns
var_pt = "^(LF|TF|GF)@([a-zA-Z]|_|-|\$|&|%|\*)(\w|_|-|\$|&|%|\*)*$"
label_pt = "^([a-zA-Z]|_|-|\$|&|%|\*)(\w|_|-|\$|&|%|\*)*$"
symb_pt = "^((LF|TF|GF)@([a-zA-Z]|_|-|\$|&|%|\*)(\w|_|-|\$|&|%|\*)*$)|(((\+|\-)?\d+)$|((true|false)$)|(([^#\s\\\\]|\\\\[0-9]{3})*)$)"
int_pt = "^(\+|\-)?\d+$"
bool_pt = "^(true|false)$"
string_pt = "([^#\s\\\\]|\\\\[0-9]{3})*$"
symb = "^(var|int|bool|string)$"
types_pt = "^(int|bool|string)$"

#Dictionaries for different types of instructions' arguments
arg_none_dict           = { 'arg_num':0}
arg_var_dict            = { 'arg_num':1, 'argtypes':['^var$'],                      'argpatterns':[var_pt] }
arg_varsymb_dict        = { 'arg_num':2, 'argtypes':['^var$', symb],                'argpatterns':[var_pt,symb_pt]}
arg_varint_dict         = { 'arg_num':2, 'argtypes':['^var$','^int$'],              'argpatterns':[var_pt, int_pt]}
arg_vartype_dict        = { 'arg_num':2, 'argtypes':['^var$', '^type$'],            'argpatterns':[var_pt, symb_pt]}  
arg_varstring_dict      = { 'arg_num':2, 'argtypes':['^var$','^string$'],           'argpatterns':[var_pt, string_pt]}
arg_varsymbsymb_dict    = { 'arg_num':3, 'argtypes':['^var$',symb,symb],            'argpatterns':[var_pt, symb_pt, symb_pt]}
arg_symb_dict           = { 'arg_num':1, 'argtypes':[symb],                         'argpatterns':[symb_pt]}
arg_label_dict          = { 'arg_num':1, 'argtypes':['^label$'],                    'argpatterns':[label_pt]}    
arg_labelsymbsymb_dict  = { 'arg_num':3, 'argtypes':['^label$',symb,symb],          'argpatterns':[label_pt, symb_pt,symb_pt]}    

#Dictionary of all operation codes, that reffer to instructions 
opcodes = { 
  'MOVE':arg_varsymb_dict,
  'CREATEFRAME':arg_none_dict,
  'PUSHFRAME':arg_none_dict,
  'POPFRAME':arg_none_dict,
  'DEFVAR':arg_var_dict,
  'CALL':arg_label_dict,
  'RETURN':arg_none_dict,

  'PUSHS': arg_symb_dict,
  'POPS':  arg_var_dict,

  'ADD':arg_varsymbsymb_dict,
  'SUB':arg_varsymbsymb_dict,
  'MUL':arg_varsymbsymb_dict,
  'IDIV':arg_varsymbsymb_dict,

  'LT':arg_varsymbsymb_dict,
  'GT':arg_varsymbsymb_dict,
  'EQ':arg_varsymbsymb_dict,

  'AND':arg_varsymbsymb_dict,
  'OR':arg_varsymbsymb_dict,
  'NOT':arg_varsymb_dict,

  'INT2CHAR': arg_varint_dict,
  'STRI2INT':arg_varsymbsymb_dict,

  'READ': arg_vartype_dict,
  'WRITE': arg_symb_dict,

  'CONCAT':arg_varsymbsymb_dict,
  'STRLEN':arg_varstring_dict,
  'GETCHAR':arg_varsymbsymb_dict,
  'SETCHAR':arg_varsymbsymb_dict,

  'TYPE': arg_varsymb_dict,

  'LABEL':arg_label_dict,
  'JUMP':arg_label_dict,
  'JUMPIFEQ':arg_labelsymbsymb_dict,
  'JUMPIFNEQ':arg_labelsymbsymb_dict,

  'DPRINT':arg_symb_dict,
  'BREAK':arg_none_dict
}

def check_regex( pattern, variable ):
  """Checks if variable matches regular expression pattern.

  Args:
      pattern:  Pattern to compare and match with.
      variable: String to compare.

  Returns:
      True  -- if matches.
      False -- if doesnt match.

  Usage:
      check_regex("^(\+|\-)?\d+$", "+100")
  """
  pattern_compared = re.compile(pattern)
  match = bool(pattern_compared.match(variable))
  if match == True:
    return True
  else:
    return False


def check_program_element():
  """ Checks syntax and correctness of main XML element 'program'.
      Exits program if error occurs.

  Args:
      None

  Returns:
      None

  Usage:
      check_program_element()
  """
  if not 'program' in tree.tag :
    exit_with_error(errcode_xml_format, "Main element name is not 'program'.")

  tree_attr_count = len(tree.attrib)

  if not 'language' in tree.attrib:
    exit_with_error(errcode_xml_format, "Attribute 'language' was not found.")
  if tree.attrib['language'] != "IPPcode18":
    exit_with_error(errcode_syntax, "Attribute 'language' contains incorrect value.")

  if tree_attr_count == 2:
      if not 'name' in tree.attrib:
        if not 'description' in tree.attrib:
          exit_with_error(errcode_xml_format, "Unknown attribute was found.")

  if tree_attr_count == 3:
    if not 'name' in tree.attrib:
      exit_with_error(errcode_xml_format, "Attribute 'name' was not found.")
    if not 'description' in tree.attrib:
      exit_with_error(errcode_xml_format, "Attribute 'description' was not found.")

  if tree_attr_count > 3 or tree_attr_count < 1:
      exit_with_error(errcode_xml_format, "Number of attributes is too big.")

labels = []

def check_instruction_element( instruction ):
  """ Checks syntax and correctness of XML elements.
      Exits program if error occurs.

  Args:
      instruction:  Instruction that represents element 'instruction'.

  Returns:
      arg_list:     List of arguments of instruction.

  Usage:
     check_instruction_element(instruction) #has to be called in loop
  """
  if not 'instruction' in instruction.tag :
    exit_with_error(errcode_xml_format, "Instruction element is not named 'instruction'. ")

  instr_attr_count = len(instruction.attrib)

  if len(instruction.attrib) != 2:
    exit_with_error(errcode_xml_format, "Instruction element contains too much or too little attributes.")

  if not 'order' in instruction.attrib:
    exit_with_error(errcode_xml_format, "Attribute 'order' in instruction element was not found.")

  if not 'opcode' in instruction.attrib:
    exit_with_error(errcode_xml_format, "Attribute 'opcode' in instruction element was not found.")

  if instruction.attrib['order'] != str(instr_counter):
    exit_with_error(errcode_syntax, "Attribute 'order' doesnt contain increasing values.")

  instr_opcode = instruction.attrib['opcode'];
  instr_arg_count = len(instruction);


  if not instr_opcode in opcodes.keys():
    exit_with_error(errcode_syntax, "Attribute 'opcode' contains unknown opcode.")
  else:
    if instr_arg_count != opcodes[instr_opcode]['arg_num']:
      exit_with_error(errcode_syntax, "Number of arguments of instruction is incorrect.")

    index=0
    index_positive=1
    arg_list = []
    for arg in instruction:
      if not 'arg'+str(index_positive) in arg.tag :
        exit_with_error(errcode_xml_format, "Arguments names are not increasing (arg1, arg2, arg3).")

      if len(arg.attrib) != 1:
        exit_with_error(errcode_xml_format, "Number of attributes in instruction argument is incorrect (should contain only 'type').")

      if not 'type' in arg.attrib:
        exit_with_error(errcode_xml_format, "Instruction argument attribute 'type' was not found.")

      if instr_opcode == "LABEL":
        for each in labels:
          if arg.text in each.keys():
            exit_with_error(errcode_semantics, "Two labels with same name defined.")
        labels.append({arg.text:instruction.attrib['order']})
        

      if not arg.text: #if empty string
        arg.text = ""

      #if not check_regex(opcodes[instr_opcode]['argpatterns'][index], arg.text):
      #  exit_with_error(errcode_syntax, "Values of arguments of instruction are incorrect.")

      #if not check_regex(opcodes[instr_opcode]['argtypes'][index], arg.attrib['type']):
      #  exit_with_error(errcode_syntax, "Types of arguments of instruction are incorrect.")
      
      check_pattern="";

      if "var" == arg.attrib['type']:
        check_pattern = var_pt
      elif "int" == arg.attrib['type']:
        check_pattern = int_pt
      elif "bool" == arg.attrib['type']:
        check_pattern = bool_pt
      elif "string" == arg.attrib['type']:
        check_pattern = string_pt
      elif "type" == arg.attrib['type']:
        check_pattern = types_pt
      elif "label" == arg.attrib['type']:
        check_pattern = label_pt
      else:
         exit_with_error(errcode_syntax, "Bad unknown type")

      if not check_regex(check_pattern, arg.text):
        exit_with_error(errcode_syntax, "Type of argument doesnt match its value.")
      
      arg = {'type': arg.attrib['type'], 'value': arg.text}
      arg_list.append(arg)
      
      index+=1
      index_positive+=1
    return arg_list
    


def split_frame_and_var( ins_args ):
  """ Splits variable value into frame and its name.

  Args:
      ins_args:   Instruction's argument that variable in format GF@variable.

  Returns:
      Pair of frame (GF,LF,TF) and variable name separated by '@'.

  Usage:
     split_frame_and_var("GF@variable")
  """
  split = ins_args['value'].split('@')
  return split[0], split[1]


tmp_frame_list = []
frame_list = []

def ins_createframe():
  """ Instruction CREATEFRAME. Creates Temporary Frame (TF) and appends it to tmp_frame_list.
  Args:     None
  Returns:  None
  Usage:
     ins_createframe()
  """
  new_tmp_frame = {'frame': 'TF'}
  if tmp_frame_list:
    tmp_frame_list.remove(tmp_frame_list[0])
    tmp_frame_list.append(new_tmp_frame)
  else:
    tmp_frame_list.append(new_tmp_frame) 

def ins_pushframe():
  """ Instruction PUSHFRAME. Moves Temporary Frame (TF) into end of list of Local Frames(LF).
  Args:     None
  Returns:  None
  Usage:
     ins_pushframe()
  """
  if tmp_frame_list:
    frame_list.append(tmp_frame_list[0])
    tmp_frame_list.remove(tmp_frame_list[0])
    frame_list[-1]['frame'] = "LF"
  else:
    exit_with_error(errcode_nonexist_frame, "TF doesnt exist.")

def ins_popframe():
  """ Instruction POPFRAME. Moves Temporary Frame (TF) into end of list of Local Frames(LF).
  Args:     None
  Returns:  None
  Usage:
     ins_pushframe()
  """
  if len(frame_list) == 1:
    exit_with_error(errcode_nonexist_frame, "LF frame doesnt exist.")
  else:
    if tmp_frame_list:
      tmp_frame_list.remove(tmp_frame_list[0])
    tmp_frame_list.append(frame_list[-1])
    frame_list.remove(frame_list[-1])
    tmp_frame_list[0]['frame'] = "TF"
 
def get_frame( myframe ):
  """ Returns relevant structure that represents frame.
  Args:     
    myframe   String that represents frame

  Returns:  
    GF, TF, LF depending on myframe value

  Usage:    
    get_frame("GF")
  """
  if myframe == "GF":
    if not global_frame:
      exit_with_error(errcode_nonexist_frame, "GF Frame doesnt exist.")
    return global_frame
  elif myframe == "LF":
    if frame_list[-1]:
      if not frame_list[-1]['frame'] == "LF":
        exit_with_error(errcode_nonexist_frame, "LF Frame doesnt exist.")
    return frame_list[-1]
  elif myframe == "TF":
    if not tmp_frame_list:
      exit_with_error(errcode_nonexist_frame, "TF Frame doesnt exist.")
    return tmp_frame_list[0]

def check_var_in_frame( var_name, var_frame ):
  """ Checks is variable name is in frame.
  Args:     
    var_name    String that represents variable
    var_frame   String that represents frame

  Returns:  
    True -- if variable is in frame
    False -- if variable is not in frame

  Usage:    
    check_var_in_frame("a", "GF")
  """
  gotframe = get_frame(var_frame)
  if var_name in gotframe:
      return True
  else:
      return False

def ins_move( ins ):
  """ Moves argument1 value into variable.
  Args:     
    ins     Instruction that represents instruction structure

  Returns:  None

  Usage:    
    ins_move(instruction)
  """
  var_frame, var_name = split_frame_and_var(ins['args'][0])
  frame_where = get_frame(var_frame)
  if check_var_in_frame(var_name, var_frame): 
    arg_type = ins['args'][1]['type']

    if arg_type == "var":
      arg_frame, arg_name = split_frame_and_var(ins['args'][1])

      if check_var_in_frame(arg_name, arg_frame):
        frame_from = get_frame(arg_frame)
        frame_where[var_name] = {'type': frame_from[arg_name]['type'],'value': frame_from[arg_name]['value']}
      else:
        exit_with_error(errcode_nonexist_var, "Variable '"+ins['args'][1]['value']+"' doesnt exist.")

    else:
      arg_value = ins['args'][1]['value']

      if arg_type == "int": #format of int 010, +010, -0000002
        arg_value = str(int(arg_value))

      frame_where[var_name] = {'type': arg_type,'value': arg_value}

  else:
    exit_with_error(errcode_nonexist_var, "Variable '"+ins['args'][0]['value']+"'' doesnt exist.")


def ins_defvar( ins ):
  """ Creates new variable in relevant frame.
  Args:     
    ins     Instruction that represents instruction structure

  Returns:  None

  Usage:    
    ins_defvar(instruction)
  """
  var_frame, var_name = split_frame_and_var(ins['args'][0])

  frame = get_frame(var_frame)

  if frame:
    if not check_var_in_frame(var_name, var_frame):
      frame[var_name] = {'type': 'Unknown', 'value': 'Unknown'}
    else:
      exit_with_error(errcode_semantics, "Variable with same name was defined.")
  else:
    exit_with_error(errcode_nonexist_frame, "Frame"+var_frame+"doenst exist.")

def ins_pushs_value (var_type, var_value):
  """ Pushes value of instruction on the stack.
  Args:     
    var_type      Type of value
    var_value     Value itself

  Returns:  None

  Usage:    
    ins_pushs_value("int", "+100")
  """
  if var_type != "Unknown":
    if var_type == "int":
      int_to_push = str(int(var_value))
      stack.append({'type':var_type, 'value':int_to_push})

    elif var_type == "bool":
      stack.append({'type':var_type, 'value':var_value})

    elif var_type == "string": 
      stack.append({'type':var_type, 'value':var_value})
  else:
    exit_with_error(errcode_missing_value, "Instruction PUSHS with variable with unknown value and type.")

def ins_pushs( ins ):
  """ Pushes value of instruction on the stack.
  Args:     
    ins     Instruction that represents instruction structure

  Returns:  None

  Usage:    
    ins_pushs(instruction)
  """
  arg_type = ins['args'][0]['type']
  arg_value = ins['args'][0]['value']

  if arg_type == "var":
    var_frame, var_name = split_frame_and_var(ins['args'][0])
    frame = get_frame(var_frame)
    if check_var_in_frame(var_name, var_frame):
      ins_pushs_value(frame[var_name]['type'], frame[var_name]['value'])
    else:
      exit_with_error(errcode_nonexist_var, "Instruction PUSHS with non existing variable.")
  else:
    ins_pushs_value(arg_type, arg_value)

def ins_pops( ins ):
  """ Pops value from stack into variable.
  Args:     
    ins     Instruction that represents instruction structure

  Returns:  None

  Usage:    
    ins_pops(instruction)
  """
  arg_type = ins['args'][0]['type']
  arg_value = ins['args'][0]['value']

  if arg_type == "var":
    var_frame, var_name = split_frame_and_var(ins['args'][0])
    frame = get_frame(var_frame)
    if check_var_in_frame(var_name, var_frame):
      if stack:
        frame[var_name]['type'] = stack[-1]['type']
        frame[var_name]['value'] = stack[-1]['value']
        del stack[-1]
      else:
        exit_with_error(errcode_missing_value, "Data stack is empty and POPS was called.")
    else:
      exit_with_error(errcode_nonexist_var, "Instruction POPS with non existing variable.")
  else:
    exit_with_error(errcode_operands, "1st argument of POPS has to be defined variable (f.e.: GF@x)")

def ins_write_value( var_type, var_value ):
  """ Writes value on STDOUT. Special dealing with escaped strings.
  Args:     
    var_type      Type of value
    var_value     Value itself

  Returns:  None

  Usage:    
    ins_write_value("string", "space\\035space" )
  """
  if var_type != "Unknown":
    if var_type == "int":
      int_to_print = var_value
      if check_regex("\+", var_value):
        int_to_print = var_value.split("+")[1]
      print(int_to_print)
    elif var_type == "bool":
       print(var_value)
    elif var_type == "string":
      hex_list = []
      esc_pattern = "\\\\[0-9]{3}"
      escaped = re.findall(esc_pattern, var_value)
      regular = re.split(esc_pattern, var_value)

      for each in escaped:
        int_value = re.split("\\\\", each)
        hex_list.append(int(int_value[1]))

      counter=0
      for escape in hex_list:
        print(regular[counter], chr(hex_list[counter]), sep='',end="")
        counter+=1

      if len(hex_list) < len(regular):
        print(regular[counter], sep='',end="") 
      print("\n",sep='',end="") 
  else:
    exit_with_error(errcode_missing_value, "Instruction WRITE with variable with unknown value and type.")

def ins_write( ins ):
  """ Writes value of instruction's arugment on STDOUT.
  Args:     
    ins     Instruction that represents instruction structure

  Returns:  None

  Usage:    
    ins_write(instruction)
  """
  arg_type = ins['args'][0]['type']
  arg_value = ins['args'][0]['value']

  if arg_type == "var":
    var_frame, var_name = split_frame_and_var(ins['args'][0])
    frame = get_frame(var_frame)
    if check_var_in_frame(var_name, var_frame):
      ins_write_value(frame[var_name]['type'], frame[var_name]['value'])
    else:
      exit_with_error(errcode_nonexist_var, "Instruction WRITE with non existing variable.")
  else:
    ins_write_value(arg_type, arg_value)

def ins_arithmetic_check_integer(  arg_type, arg_value, arg_order):
  """ Checks if argument of instruction is integer value.
  Args:     
    arg_type      Integer type "int", or variable "var"
    arg_value     ARgument's value
    arg_order     Argument's order

  Returns:  
    Integer value.

  Usage:    
    ins_arithmetic_check_integer("int", "-0002", 0)
  """
  if arg_type == "var":
      arg_var_frame, arg_var_name = split_frame_and_var(ins['args'][arg_order])
      arg_frame = get_frame(arg_var_frame)
      if check_var_in_frame(arg_var_name, arg_var_frame):
        if arg_frame[arg_var_name]['type'] == "int":
          return arg_frame[arg_var_name]['value']
        else:
          exit_with_error(errcode_operands, "Operands in arithemetic operation are not integers.")
      else:
        exit_with_error(errcode_nonexist_var, "Variable used in arithemetic operation doesnt exist.")
  elif arg_type == "int":
    return arg_value
  else:
    exit_with_error(errcode_operands, "Operands in arithemetic operation are not integers.")

def ins_arithmetic_do ( ins, op1, op2 ):
  """ Determines arithmetic operation and executes it.
  Args:     
    ins     Instruction that represents instruction structure
    op1     First integer operand
    op2     Second integer operand

  Returns:  
    Result of arithmetic operation as Integer value.

  Usage:    
    ins_arithmetic_do(instruction, 10, 20)
  """
  opcode = ins['opcode']
  if opcode == "ADD":
    return op1+op2
  elif opcode == "SUB":
    return op1-op2
  elif opcode == "MUL":
    return op1*op2
  elif opcode == "IDIV":
    if op2 != 0:
      return op1//op2
    else:
      exit_with_error(errcode_div_by_zero, "Division by zero in arithemetic instruction.")

def ins_arithmetic( ins ):
  """ Executes arithmetic operation. Fills variable with result of that operation.
  Args:     
    ins     Instruction that represents instruction structure

  Returns:  None

  Usage:    
    ins_arithmetic(instruction)
  """
  var_frame, var_name = split_frame_and_var(ins['args'][0])

  arg1_type = ins['args'][1]['type']
  arg1_value = ins['args'][1]['value']
  arg2_type = ins['args'][2]['type']
  arg2_value = ins['args'][2]['value']

  if check_var_in_frame(var_name, var_frame):
    operand1 = int(ins_arithmetic_check_integer(arg1_type, arg1_value,1))
    operand2 = int(ins_arithmetic_check_integer(arg2_type, arg2_value,2))
    result = ins_arithmetic_do(ins, operand1, operand2)
    frame = get_frame(var_frame)

    frame[var_name]['type'] = "int";
    frame[var_name]['value'] = str(result);

  else:
    exit_with_error(errcode_nonexist_var, "Varibale doesnt exist in frame.")

def ins_relational_modif(op1, op2, optype):
  """ Modifies value depending on their type.
  Args:     
    optype  Type of both operands
    op1     First integer operand
    op2     Second integer operand

  Returns:  
    Modified operands.

  Usage:    
    ins_relational_modif("10", "20", "int")
  """
  if optype == "int":
    return int(op1), int(op2)

  elif optype == "bool":
    if op1 == "false":
      if op2 == "false":
        return 0, 0
      else:
        return 0, 1
    else:
      if op2 == "false":
        return 1, 0
      else:
        return 1, 1
  elif optype == "string":
    return op1, op2

def ins_relational_do ( ins, op1, op2, optype ):
  """ Compares two operands and return its result.
  Args:     
    ins     Instruction that represents instruction structure
    op1     First operand
    op2     Second operand
    optype  Type of both operands

  Returns:  
    Result of relational operation.

  Usage:    
    ins_relational_do(instruction,"10", "20", "int")
  """
  opcode = ins['opcode']
  op1, op2 = ins_relational_modif(op1, op2, optype)
  if opcode == "LT":
    return op1<op2
  elif opcode == "GT":
    return op1>op2
  elif opcode == "EQ":
    return op1==op2

def ins_relational_check_operand( arg_type, arg_value, arg_order ):
  """ Checks if argument of instruction is variable.
  Args:     
    arg_type      Variable "var"
    arg_value     ARgument's value
    arg_order     Argument's order

  Returns:  
    Variable type and value.

  Usage:    
    ins_relational_check_operand("var", "GF@a", 0)
  """
  if arg_type == "var":
    arg_var_frame, arg_var_name = split_frame_and_var(ins['args'][arg_order])
    arg_frame = get_frame(arg_var_frame)
    if check_var_in_frame(arg_var_name, arg_var_frame):
      return arg_frame[arg_var_name]['type'], arg_frame[arg_var_name]['value']
    else:
      exit_with_error(errcode_nonexist_var, "Variable used in arithemetic operation doesnt exist.")
  else:
    return arg_type, arg_value
  
def bool_to_string( boolvar ):
  """ Covnerts boolean value into string.
  Args:     
    boolvar       Boolean value (True or False)

  Returns:  
    Depending on Boolean value "true" or "false".

  Usage:    
    bool_to_string(True)
  """
  if boolvar == True:
    return "true"
  elif boolvar == False:
    return "false"
  else:
    exit_with_error(errcode_operands, " Function bool_to_string(), argument is not bool.")

def ins_relational( ins ):
  """ Executes relational operation. Fills variable with result of that operation.
  Args:     
    ins     Instruction that represents instruction structure

  Returns:  None

  Usage:    
    ins_relational(instruction)
  """
  var_frame, var_name = split_frame_and_var(ins['args'][0])

  arg1_type = ins['args'][1]['type']
  arg1_value = ins['args'][1]['value']

  arg2_type = ins['args'][2]['type']
  arg2_value = ins['args'][2]['value']

  if check_var_in_frame(var_name, var_frame):
    op1_type, op1_value = ins_relational_check_operand(arg1_type, arg1_value,1)
    op2_type, op2_value = ins_relational_check_operand(arg2_type, arg2_value,2)
    if op1_type == op2_type:
      
      result = ins_relational_do(ins, op1_value, op2_value, op1_type)
      frame = get_frame(var_frame)

      frame[var_name]['type'] = "bool";
      frame[var_name]['value'] = bool_to_string(result);
      
    else:
      exit_with_error(errcode_operands, "Relational instruction operands' types collision.")
  else:
    exit_with_error(errcode_nonexist_var, "Varibale doesnt exist in frame.")

def string_to_bool(stringvar):
  """ Covnerts string value into bool.
  Args:     
    stringvar       String value ("true" or "false")

  Returns:  
    Depending on String value True or  False.

  Usage:    
    string_to_bool("true")
  """
  if stringvar == "false":
    return False
  elif stringvar == "true":
    return True
  else: 
    exit_with_error(errcode_operands,"Function string_to_bool(), argument is not string representing bool.")

def ins_boolean_do(ins, op1, op2):
  """ Chooses operation and compares two operands and return its result.
  Args:     
    ins     Instruction that represents instruction structure
    op1     First boolean operand
    op2     Second boolean operand

  Returns:  
    Result of boolean operation.

  Usage:    
    ins_boolean_do(instruction, "true", "false")
  """
  opcode = ins['opcode']
  if opcode == "AND":
    return string_to_bool(op1) and string_to_bool(op2)
  elif opcode == "OR":
    return string_to_bool(op1) or string_to_bool(op2)
  elif opcode == "NOT":
    return not string_to_bool(op1) 

def ins_boolean_check_operand(arg_type, arg_value, arg_order):
  """ Checks if argument of instruction is boolean value.
  Args:     
    arg_type      Boolean type "bool", or variable "var"
    arg_value     Argument's value
    arg_order     Argument's order

  Returns:  
    Boolean value.

  Usage:    
    ins_boolean_check_operand("bool", "true", 0)
  """
  if arg_type == "var":
    arg_var_frame, arg_var_name = split_frame_and_var(ins['args'][arg_order])
    arg_frame = get_frame(arg_var_frame)
    if check_var_in_frame(arg_var_name, arg_var_frame):
      if arg_frame[arg_var_name]['type'] == "bool":
        return arg_frame[arg_var_name]['value']
      else:
        exit_with_error(errcode_operands, "Operands in boolean operation are not bool values.")
    else:
      exit_with_error(errcode_nonexist_var, "Variable used in boolean operation doesnt exist.")
  elif arg_type == "bool":
    return arg_value
  else:
    exit_with_error(errcode_operands, "Operands in boolean operation are not booleans.")

def ins_boolean( ins ):
  """ Executes boolean operation. Fills variable with result of that operation.
  Args:     
    ins     Instruction that represents instruction structure

  Returns:  None

  Usage:    
    ins_boolean(instruction)
  """
  var_frame, var_name = split_frame_and_var(ins['args'][0])

  arg1_type = ins['args'][1]['type']
  arg1_value = ins['args'][1]['value']
  arg_num = ins['argnum']

  if arg_num == 3:
    arg2_type = ins['args'][2]['type']
    arg2_value = ins['args'][2]['value']

  if check_var_in_frame(var_name, var_frame):
    operand1 = ins_boolean_check_operand(arg1_type, arg1_value,1)
    operand2 = None
    if arg_num == 3:
      operand2 = ins_boolean_check_operand(arg2_type, arg2_value,2)

    result = ins_boolean_do(ins, operand1, operand2)
    frame = get_frame(var_frame)

    frame[var_name]['type'] = "bool";
    frame[var_name]['value'] = bool_to_string(result);

  else:
    exit_with_error(errcode_nonexist_var, "Variable doesnt exist in frame.")


def ins_int2char( ins ):
  """ Represents ins INT2CHAR. Converts integer value into character. Fills variable with character string value.
  Args:     
    ins     Instruction that represents instruction structure

  Returns:  None

  Usage:    
    ins_int2char(instruction)
  """
  var_frame, var_name = split_frame_and_var(ins['args'][0])
  arg1_type = ins['args'][1]['type']
  arg1_value = ins['args'][1]['value']

  if check_var_in_frame(var_name, var_frame):
    op1_type = arg1_type
    op1_value = int(ins_arithmetic_check_integer(arg1_type, arg1_value,1))
    if op1_value in range(0,256):
      result = chr(op1_value)
      frame = get_frame(var_frame)
      frame[var_name]['type'] = "string"
      frame[var_name]['value'] = result
    else:
      exit_with_error(errcode_string, "ASCII value of argument in instruction INT2CHAR is different from [0..255].")
  else:
    exit_with_error(errcode_nonexist_var, "Varibale doesnt exist in frame.")

def ins_strings_check_string( arg_type, arg_value, arg_order ):
  """ Checks if argument of instruction is string value.
  Args:     
    arg_type      String type "string", or variable "var"
    arg_value     Argument's value
    arg_order     Argument's order

  Returns:  
    String value.

  Usage:    
    ins_boolean_check_operand("string", "hello", 0)
  """
  if arg_type == "var":
    arg_var_frame, arg_var_name = split_frame_and_var(ins['args'][arg_order])
    arg_frame = get_frame(arg_var_frame)
    if check_var_in_frame(arg_var_name, arg_var_frame):
      if arg_frame[arg_var_name]['type'] == "string":
        return arg_frame[arg_var_name]['value']
      else:
        exit_with_error(errcode_operands, "Operands in string operation are not bool values.")
    else:
      exit_with_error(errcode_nonexist_var, "Variable used in string operation doesnt exist.")
  elif arg_type == "string":
    return arg_value
  else:
    exit_with_error(errcode_operands, "Operands in string operation are not strings.")

def ins_stri2int( ins ):
  """ Represents ins STRI2INT. Converts character on index into integer value. 
      Fills variable with integer value of that characeter.
  Args:     
    ins     Instruction that represents instruction structure

  Returns:  None

  Usage:    
    ins_stri2int(instruction)
  """
  var_frame, var_name = split_frame_and_var(ins['args'][0])
  arg1_type = ins['args'][1]['type']
  arg1_value = ins['args'][1]['value']
  arg2_type = ins['args'][2]['type']
  arg2_value = ins['args'][2]['value']

  if check_var_in_frame(var_name, var_frame):
    op_string = ins_strings_check_string(arg1_type, arg1_value, 1)
    op_index = int(ins_arithmetic_check_integer(arg2_type, arg2_value,2))
    op_string_len = escaped_string_len(op_string)

    char_counter=0
    esc_counter=0
    esc_sequence=False
    c=[]
    if op_index < op_string_len and op_index >= 0:
      frame = get_frame(var_frame)
      while (char_counter<= op_index):
        if op_string[char_counter] == "\\":
          if char_counter == op_index:
            esc_sequence=True
            esc_counter+=1
            while (esc_counter < 3):
              c.append(op_string[op_index+esc_counter])
              esc_counter+=1
          char_counter+=3
          op_index+=3
        else:
          if char_counter == op_index:
            c.append(op_string[op_index])
          char_counter+=1

      c = "".join(c)
      if esc_sequence:
        c = int(c)
      else:
        c = ord(c)     

      frame[var_name]['type'] = "int";
      frame[var_name]['value'] = str(c);
    else:
       exit_with_error(errcode_string, "Index out of borders of string.")
  else:
    exit_with_error(errcode_nonexist_var, "Varibale doesnt exist in frame.")

def ins_read( ins ):
  """ Read value from standard input. Fills variable wih read value.
  Args:     
    ins     Instruction that represents instruction structure

  Returns:  None

  Usage:    
    ins_read(instruction)
  """
  var_frame, var_name = split_frame_and_var(ins['args'][0])
  arg_type = ins['args'][1]['value'] #attention value is INT, STRING or BOOL
  if check_var_in_frame(var_name, var_frame):
    arg_value = input()
    frame = get_frame(var_frame)
    if arg_type == "string":

      if arg_value and check_regex(string_pt, arg_value):
        frame[var_name]['type'] = "string"
        frame[var_name]['value'] = str(arg_value)
      else:
        frame[var_name]['type'] = "string"
        frame[var_name]['value'] = ""
    elif arg_type == "int":
      try:
        frame[var_name]['type'] = "int"
        frame[var_name]['value'] = str(int(arg_value))
      except:
        frame[var_name]['type'] = "int"
        frame[var_name]['value'] = "0"
    elif arg_type == "bool":
      if arg_value.lower() == "true":
        frame[var_name]['type'] = "bool"
        frame[var_name]['value'] = "true"
      else:
        frame[var_name]['type'] = "bool"
        frame[var_name]['value'] = "false"
  else:
    exit_with_error(errcode_nonexist_var, "Varibale doesnt exist in frame.")



def ins_concat( ins ):
  """ Concatenates two string values. Fills variable wih that string value.
  Args:     
    ins     Instruction that represents instruction structure

  Returns:  None

  Usage:    
    ins_concat(instruction)
  """
  var_frame, var_name = split_frame_and_var(ins['args'][0])
  frame = get_frame(var_frame)
  arg1_type = ins['args'][1]['type']
  arg1_value = ins['args'][1]['value']
  arg2_type = ins['args'][2]['type']
  arg2_value = ins['args'][2]['value']

  if check_var_in_frame(var_name, var_frame):
    operand1 = ins_strings_check_string(arg1_type, arg1_value,1)
    operand2 = ins_strings_check_string(arg2_type, arg2_value,2)
    result = operand1+operand2

    frame[var_name]['type'] = "string"
    frame[var_name]['value'] = result
  else:
    exit_with_error(errcode_nonexist_var, "Variable doesnt exist in frame.")

def escaped_string_len(op_string):
  """ Returns length of string that contains escape sequences.
  Args:     
    op_string       String to count its length

  Returns:  None

  Usage:    
    escaped_string_len("\\035abcd\\035") [length is 6]
  """
  esc_pattern = "\\\\[0-9]{3}"
  escaped = re.findall(esc_pattern, op_string)
  regular = re.split(esc_pattern, op_string)
  op_string_len=0
  for each in escaped:
    op_string_len+=1

  for each in regular:
    op_string_len+=len(each)

  return op_string_len


def ins_getchar(ins):
  """ Gets value of character on index. Fills variable with that character.
  Args:     
    ins     Instruction that represents instruction structure

  Returns:  None

  Usage:    
    ins_getchar(instruction)
  """
  var_frame, var_name = split_frame_and_var(ins['args'][0])
  arg1_type = ins['args'][1]['type']
  arg1_value = ins['args'][1]['value']
  arg2_type = ins['args'][2]['type']
  arg2_value = ins['args'][2]['value']

  if check_var_in_frame(var_name, var_frame):
    op_string = ins_strings_check_string(arg1_type, arg1_value,1)
    op_index = int(ins_arithmetic_check_integer(arg2_type, arg2_value,2))
    op_string_len = escaped_string_len(op_string)

    char_counter=0
    esc_counter=0
    c=[]
    if op_index < op_string_len and op_index >= 0:
      frame = get_frame(var_frame)
      while (char_counter<= op_index):
        if op_string[char_counter] == "\\":
          if char_counter == op_index:
            while (esc_counter < 3):
              c.append(op_string[op_index+esc_counter])
              esc_counter+=1
          char_counter+=3
          op_index+=3
        else:
          if char_counter == op_index:
            c.append(op_string[op_index])
          char_counter+=1

      c = "".join(c)
      frame[var_name]['type'] = "string";
      frame[var_name]['value'] = c       
    else:
       exit_with_error(errcode_string, "Index out of borders of string.")
  else:
    exit_with_error(errcode_nonexist_var, "Varibale doesnt exist in frame.")

def ins_setchar(ins):
  """ Changes value of character on index of variable string value.
  Args:     
    ins     Instruction that represents instruction structure

  Returns:  None

  Usage:    
    ins_setchar(instruction)
  """
  var_frame, var_name = split_frame_and_var(ins['args'][0])
  arg1_type = ins['args'][1]['type']
  arg1_value = ins['args'][1]['value']
  arg2_type = ins['args'][2]['type']
  arg2_value = ins['args'][2]['value']

  frame = get_frame(var_frame)
  var_type = frame[var_name]['type']
  var_value = frame[var_name]['value']

  if check_var_in_frame(var_name, var_frame):
    if var_type == "string":
      op_index = int(ins_arithmetic_check_integer(arg1_type, arg1_value,1))
      op_string = ins_strings_check_string(arg2_type, arg2_value,1)
      var_string_len = len(var_value)
      op_string_len = len(op_string)

      if op_string_len > 0:
        if op_index < var_string_len and op_index >= 0:
          var_value = list(var_value)

          if var_value[op_index] == "\\":
            if op_string[0] == "\\":
              for i in range(1,4):
                var_value[op_index+i] = op_string[i]  
            else:   
              for i in range(1,4):
                del var_value[op_index+1] 
              var_value[op_index] = op_string[0]
          else:
            if op_string[0] == "\\":
              var_value[op_index] = "\\"
              for i in range(1,4):
                var_value.insert(op_index+i, op_string[i])
            else:
              var_value[op_index] = op_string[0]

          var_value = "".join(var_value)
          frame[var_name]['value'] = str(var_value)
        else:
           exit_with_error(errcode_string, "Index out of borders of string.")
      else:
        exit_with_error(errcode_string, "Empty string to insert in SETCHAR.")
    else:
      exit_with_error(errcode_operands, "In variable there is no string.")
  else:
    exit_with_error(errcode_nonexist_var, "Varibale doesnt exist in frame.")

def ins_strlen(ins):
  """ Gets length of string. Uses escaped_string_length(). Fills variable with length value.
  Args:     
    ins     Instruction that represents instruction structure

  Returns:  None

  Usage:    
    ins_strlen(instruction)
  """
  var_frame, var_name = split_frame_and_var(ins['args'][0])
  arg_type = ins['args'][1]['type']
  arg_value = ins['args'][1]['value']
  frame = get_frame(var_frame)

  if check_var_in_frame(var_name, var_frame):
    op_string = ins_strings_check_string(arg_type, arg_value,1)
    op_string_len = escaped_string_len(op_string)
    frame[var_name]['type'] = "int";
    frame[var_name]['value'] = str(op_string_len)
  else:
    exit_with_error(errcode_nonexist_var, "Varibale doesnt exist in frame.")

def ins_type_get(arg_type, arg_value):
  """ Gets type of variable or value itself.
  Args:     
    arg_type     Argument's type
    arg_value    Argument's value

  Returns:  
    Type of variable or value.

  Usage:    
    ins_type_get(instruction)
  """
  if arg_type == "var":
    arg_var_frame, arg_var_name = split_frame_and_var(ins['args'][1])
    arg_frame = get_frame(arg_var_frame)
    if check_var_in_frame(arg_var_name, arg_var_frame):
      return arg_frame[arg_var_name]['type']
    else:
      exit_with_error(errcode_nonexist_var, "Variable used in string operation doesnt exist.")
  elif arg_type =="int":
    ins_arithmetic_check_integer(arg_type, arg_value, 1)
  elif arg_type =="bool":
    ins_boolean_check_operand(arg_type, arg_value, 1)
  elif arg_type =="string":
    ins_strings_check_string(arg_type, arg_value, 1)
  else:
    exit_with_error(errcode_semantics, "Unknown type for operand in function TYPE.")
  return arg_type


def ins_type(ins):
  """ Represents instruction TYPE. Gets type of variable or value itself.
  Args:     
    ins     Instruction that represents instruction structure

  Returns:  None

  Usage:    
    ins_type(instruction)
  """
  var_frame, var_name = split_frame_and_var(ins['args'][0])
  arg1_type = ins['args'][1]['type']
  arg1_value = ins['args'][1]['value']
  frame = get_frame(var_frame)

  if check_var_in_frame(var_name, var_frame):
    var_type = ins_type_get(arg1_type, arg1_value)
    if var_type == "Unknown": #if not initialized variable GF@x for example
      var_type = ""

    frame[var_name]['type'] = "string";
    frame[var_name]['value'] = var_type
  else:
    exit_with_error(errcode_nonexist_var, "Varibale doesnt exist in frame.") 

def ins_jump( ins, program_counter ):
  """ Jumps on relevant label that exists in program.
  Args:     
    ins     Instruction that represents instruction structure
    program_counter Number of executed instruction

  Returns:  
    Value of modified program_counter after jump on label.

  Usage:    
    ins_jump(instruction, program_counter)
  """
  arg_type = ins['args'][0]['type']
  arg_value = ins['args'][0]['value']
  found = False
  index=0
  for each in labels:
    if arg_value in each.keys():
      found = True
      ins_label = arg_value
      ins_order = int(labels[index][arg_value])
    index+=1 

  if found == True:
    if program_counter < ins_order:
      while(program_counter<ins_order):
        program_counter+=1
      return program_counter
    else:
      while(program_counter>ins_order):
        program_counter-=1
      return program_counter
  else:
    exit_with_error(errcode_semantics, "Instruction LABEL with unknown label.")

def ins_jumpif_eq_neq(ins, program_counter, eq):
  """ Jumps on relevant label that exists in program if values of arguments are equal or not.
  Args:     
    ins     Instruction that represents instruction structure
    program_counter Number of executed instruction
    eq      Vaue that represents instrcution JUMPIFEQ or JUMPIFNEQ

  Returns:  
    Value of modified program_counter after jump on label.

  Usage:    
    ins_jumpif_eq_neq(instruction, program_counter, "EQ")
  """
  arg0_type = ins['args'][0]['type']
  arg0_value = ins['args'][0]['type']
 
  arg1_type = ins['args'][1]['type']
  arg1_value = ins['args'][1]['value']

  arg2_type = ins['args'][2]['type']
  arg2_value = ins['args'][2]['value']

  if arg0_type == "label":
    if check_regex(label_pt, arg0_value):
      op1_type, op1_value = ins_relational_check_operand(arg1_type, arg1_value,1)
      op2_type, op2_value = ins_relational_check_operand(arg2_type, arg2_value,2)
      if op1_type == op2_type:
        if eq == "EQ":
          if op1_value == op2_value:
            return ins_jump(ins, program_counter)
          else:
            return program_counter
        elif eq == "NEQ":
          if op1_value != op2_value:
            return ins_jump(ins, program_counter)
          else:
            return program_counter
      else:
        exit_with_error(errcode_operands, "Relational instruction operands' types collision.")
    else:
      exit_with_error(errcode_nonexist_var, "Varibale doesnt exist in frame.")
  else:
    exit_with_error(errcode_operands, "Label name is not type label.")

def ins_call(ins, program_counter ):
  """ Calls the function on relevant label. Jumps on relevant label that exists in program.
  Args:     
    ins     Instruction that represents instruction structure
    program_counter Number of executed instruction

  Returns:  
    ins_jump() function -- Value of modified program_counter after jump on label.

  Usage:    
    ins_jumpif_eq_neq(instruction, program_counter, "EQ")
  """

  return ins_jump(ins, program_counter)

#Checks 'program' element 
check_program_element()
  
#Data stack
stack = []

#Global frame
global_frame = {'frame': 'GF'}
frame_list.append(global_frame)

#Instruction counter
instr_counter = 0

#Instruction list
instr_list = []

#Fill instruction list and checks structure of XML tree and elements values
for instruction in tree:
  instr_counter+=1
  new_instr = {
    'order': instruction.get('order'), 
    'opcode': instruction.get('opcode'), 
    'argnum': len(instruction),
    'args': check_instruction_element(instruction)
  }
  instr_list.append(new_instr);



def print_frames_lists():
  """ Debug function to write out GF, LFs and TF frames
  Args:  None
  Returns:  None
  Usage:    
    print_frames_lists()
  """
  for each in frame_list:
    print(each['frame'], ":")
    for key in each.keys():
      if key != "frame":
        print("   ", each['frame'],"@", key, " = " ,each[key]['value'], " (",each[key]['type'],")", sep="")

  for each in tmp_frame_list:
    print(each['frame'], ":")
    for key in each.keys():
      if key != "frame":
        print("   ", each['frame'],"@", key, " = " ,each[key]['value'], " (",each[key]['type'],")", sep="")

  print("STACK:  ")
  for each in stack:
    print("  ", each['type'],each['value'])


def print_instruction(ins):
  """ Debug function to write out instruction.
  Args:  None
  Returns:  None
  Usage:    
    print_instruction()
  """
  argnum = ins['argnum']

  print("\n****************************************************************************")
  print(ins['opcode'],sep="", end=" ")
  if argnum > 0:
    print(ins['args'][0]['value'],sep="", end=" ")
    if argnum > 1:
        print(ins['args'][1]['value'],sep="", end=" ")
        if argnum > 2:
          print(ins['args'][2]['value'],sep="", end=" ")
  print("\n****************************************************************************\n")


#List of function callback values
function_back = []
function_calls = []
program_counter = 0

drist=0

#Main program Interpreting the instructions.
while (program_counter < instr_counter):
  ins = instr_list[program_counter]


  #drist+=1
  #if drist > 20:
  #  exit(0)


  #print_frames_lists()
  #print_instruction(ins)
  #print("\n")
  #print("INSTRUCTON:  ",ins)
  program_counter+=1

  #defvar
  if ins['opcode'] == "DEFVAR":
    ins_defvar(ins)

  #move
  elif ins['opcode'] == "MOVE":
    ins_move (ins)
  
  #createframe
  elif ins['opcode'] == "CREATEFRAME":
    ins_createframe()

  #pushframe
  elif ins['opcode'] == "PUSHFRAME":
    ins_pushframe()

  #popframe
  elif ins['opcode'] == "POPFRAME":
    ins_popframe()

  #pushs
  elif ins['opcode'] == "PUSHS":
    ins_pushs(ins)

  #pops
  elif ins['opcode'] == "POPS":
    ins_pops(ins)    

  #add, sub, mul, idiv
  elif ins['opcode'] in ("ADD", "SUB", "MUL", "IDIV"):
    ins_arithmetic(ins)  

  #alet, gt, eq
  elif ins['opcode'] in ("LT", "GT", "EQ"):
    ins_relational(ins) 

  #and, or, not
  elif ins['opcode'] in ("AND", "OR", "NOT"):
    ins_boolean(ins)  

  #int2char
  elif ins['opcode'] == "INT2CHAR":
    ins_int2char(ins)

  #stri2int
  elif ins['opcode'] == "STRI2INT":
    ins_stri2int(ins)

  #read
  elif ins['opcode'] == "READ":
    ins_read(ins)

  #write
  elif ins['opcode'] == "WRITE":
    ins_write(ins)

  #concat
  elif ins['opcode'] == "CONCAT":
    ins_concat(ins)

  #strlen
  elif ins['opcode'] == "STRLEN":
    ins_strlen(ins)

  #getchar
  elif ins['opcode'] == "GETCHAR":
    ins_getchar(ins)

  #setchar
  elif ins['opcode'] == "SETCHAR":
    ins_setchar(ins)

  #type
  elif ins['opcode'] == "TYPE":
    ins_type(ins)

  #call
  elif ins['opcode'] == "CALL":

    #print_frames_lists()
    #print("\n")
    function_calls.append(True)
    function_back.append(program_counter)

    #print("CALL", function_back)
    #print("CALL:",function_calls)

    program_counter=ins_call(ins, program_counter)

  #return
  elif ins['opcode'] == "RETURN":
    if function_calls:
      #print_frames_lists()
      #print("\n")
      program_counter=function_back[-1]
      function_calls.remove(function_calls[-1])
      function_back.remove(function_back[-1])
      #print("RETURN", function_back)
      #print_frames_lists()
      #print("RETURN:",function_calls)
    else:
      exit_with_error(errcode_missing_value, "Return called outside the function.")

  #jump
  elif ins['opcode'] == "JUMP":
    program_counter=ins_jump(ins, program_counter)

  #jumpifeq
  elif ins['opcode'] == "JUMPIFEQ":
    program_counter=ins_jumpif_eq_neq(ins, program_counter, "EQ")

  #jumpifneq
  elif ins['opcode'] == "JUMPIFNEQ":
    program_counter=ins_jumpif_eq_neq(ins, program_counter, "NEQ")

  #label
  elif ins['opcode'] == "LABEL":
    ... #useless

  #dprint
  elif ins['opcode'] == "DPRINT":
    ...

  #break
  elif ins['opcode'] == "BREAK":
    ...
