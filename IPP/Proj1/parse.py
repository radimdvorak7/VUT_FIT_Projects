#FILE: parse.py
#AUTHOR: Radim Dvořák xdvorar00
#DATE: 2025/03/11

import re
import sys
import xml.etree.ElementTree as ElementTree
from xml.dom import minidom
from lark import exceptions, Lark, Tree, Token, Transformer, Visitor, ParseTree, visitors

classes = ["Object","Nil","Integer","String","Block","True","False"]

def LexicalError():
    print("Lexical error was found",file=sys.stderr)
    sys.exit(21)

def SyntaxError():
    print("Syntatic error was found", file=sys.stderr)
    sys.exit(22)

def SemError(code:int):
    print(f"Semantic error was found with code: {code}", file=sys.stderr)
    sys.exit(code)

def selectorParam(token:list):
    final_selector = ""
    if not token:
        return ""
    selector = token.pop(0)
    if not token:
        return selector
    
    param_list = token.pop(0).children
    final_selector = selector + selectorParam(param_list)
    return final_selector

def checkLiteralClass(literal:Token):
    if re.fullmatch("false",literal):
        return "False"
    elif re.fullmatch("true",literal):
        return "True"
    elif re.fullmatch("nil",literal):
        return "Nil"
    elif re.fullmatch("[0-9]*",literal):
        return "Integer"
    elif re.fullmatch("'(\\'|[^'])*'",literal):   
        return "String"
    elif re.fullmatch("[A-Z][a-zA-Z0-9]*",literal):   
        return "class"
    else:
        return "var"

class KeywordCheck(Visitor):
    def assign(self,token):
        if re.fullmatch("class|self|super|nil|true|false",token.children[0]):
            SyntaxError()
    def method_selector(self,token):
        if re.fullmatch("class|self|super|nil|true|false",token.children[0]):
            SyntaxError()
    def parameter_name(self,token):
        param = token.children[0][1:]
        if re.fullmatch("class|self|super|nil|true|false",param):
            SyntaxError()
    def expr_sel(self,token):
        selector = token.children.copy()
        if selector:
            selector_name = selector.copy().pop()
            if isinstance(selector_name,Token) and re.fullmatch("class|self|super|nil|true|false",selector_name):
                SyntaxError()

class SemCreate(Transformer):
    def P_ID(self, token):
        return Token(token.type, token.value.lstrip(':'))
    def start(self,token:list):
        main_found = False
        for class_name in token:
            if class_name.data == "class":
                for class_attr in class_name.children:
                    if class_attr.data == "class_name":
                        classes.append(class_attr.children[0].value)
                    if class_attr.children[0] == "Main":  
                        main_found = True
        if not main_found:
            SemError(31)
        for class_name in token:
            if class_name.data == "class":
                for class_attr in class_name.children:
                    if class_attr.data == "class_parent" and not classes.count(class_attr.children[0]):
                        SemError(32)
        return Tree(Token("RULE","program"),token)
    def class_def(self,token:list):
        method_list = token[2:]
        run_found = False
        if token[0].children[0] == "Main":
            for method in method_list:
                if method.children[0].children[0] == "run":
                    run_found = True
            if not run_found:
                SemError(31)
        return Tree(Token("RULE","class"),token)
    def method(self,token:list):
        selector_name = token.copy().pop(0).children[0]
        block_arity = token.copy().pop(1).children.copy().pop(0).children[0]
        try:
            block_arity = int(block_arity)
        except ValueError:
            SemError(33)
        if selector_name.count(":") != block_arity:
            SemError(33)
        return Tree(Token("RULE","method"),token)
    def method_selector(self,token:list):
        final_selector = selectorParam(token)
        return Tree(Token("RULE","method_selector"),[Token("RULE",final_selector)])
    def block(self,token:list):
        tree = Tree("block",token)
        assigns = tree.find_data("assign")
        assign_count = 0
        for assign in assigns:
            var_name = assign.children[0]
            assign.children.remove(var_name)
            assign.children.insert(0,Tree(Token("SEM","assign_order"),[Token('INT', assign_count + 1)]))
            assign.children.insert(0,Tree(Token("RULE","var"),[Tree(Token('SEM', 'var_name'),Token('VAR', var_name))]))
            assign_count += 1
        parameters = tree.find_data("parameter")
        par_count = 0
        for par in parameters:
            par.children.insert(par_count,Tree(Token("SEM","parameter_order"),Token("INT",par_count + 1)))
            par_count += 1
        new_token = Tree(Token('SEM', 'block_arity'),[Token('INT', par_count)])
        for i, child in enumerate(tree.children):
            if isinstance(child, Tree) and child.data == 'parameter':
                tree.children.insert(i, new_token)
                return Tree(Token("RULE","block"),token)
        tree.children.insert(0, new_token)
        return Tree(Token("RULE","block"),token)
    def expr_tail(self,token:list):
        if not token:
            return visitors.Discard
        return Tree(Token("RULE","send"),token)
    def expr_base(self,token:list):
        literal = token.copy().pop()
        if isinstance(literal, Tree) and literal.data == "block":
            return Tree(Token("RULE","expr"),token)
        literal_class = checkLiteralClass(literal)
        if literal_class == "var":
            return Tree(Token("RULE","var"),[Tree(Token('SEM', 'var_name'),[literal])])
        elif literal_class == "String":
            literal = literal[1:-1]
        return Tree(Token("RULE","literal"),[Tree(Token('SEM', 'literal_value'),[literal]),Tree(Token('SEM', 'literal_class'),[literal_class])])
    def expr(self,token:list):
        send = token.copy().pop()
        base = token.copy().pop(0)
        
        next = send.children.copy().pop(0).children
        if not next:
            return Tree(Token("RULE","expr"),[base])
        id = next.copy().pop(0)
        selector_args = next.copy()
        print(selector_args[2])
        arg_count = 0
        arg_param = 1
        arg_list = []
        for arg in selector_args:
            if isinstance(arg,Tree) and arg.data == "literal":
                arg_list.append(Tree(Token("SEM","arg"),[Tree(Token("SEM","arg_order"),[Token("INT",arg_count)]),selector_args.copy().pop(arg_param)]))
                arg_param += 2
                arg_count += 1
        if not isinstance(selector_args,Token) and not selector_args:
            return Tree(Token("RULE","expr"),[Tree(Token("RULE","send"),[Tree(Token("RULE","send_selector"),[Token("ID",id)]),Tree(Token("RULE","expr"),[base])])])
        if isinstance(selector_args.copy().pop(),Token):
            return Tree(Token("RULE","expr"),[Tree(Token("RULE","send"),[Tree(Token("RULE","send_selector"),[Token("ID",id)]),Tree(Token("RULE","expr"),[base])])])
        return Tree(Token("RULE","expr"),[Tree(Token("RULE","send"),arg_list + [Tree(Token("RULE","send_selector"),[Token("ID",id)]),Tree(Token("RULE","expr"),[base])])])
    def var(self,token:list):
        var = token.copy().pop()
        return Tree(Token("RULE","var"),[Tree(Token('SEM', 'var_name'),[var])])
    def assign(self,token:list):
        return Tree(Token("RULE","assign"),token)

    
def to_xml(tree: ParseTree):
    element = ElementTree.Element(tree.data)
    for child in tree.children:
        if isinstance(child, str):
            element.text = child
        else:
            element.append(to_xml(child))
    return element

if __name__=="__main__":
    comments = []

    grammar = """

    start: class_def*

    class_def: "class" class_name ":" class_parent "{" method* "}"

    class_name: CLASS_ID
    class_parent: CLASS_ID

    method:  method_selector block

    method_selector: ID 
    | S_ID selector_tail
    
    selector_tail: S_ID selector_tail
    |

    block: "[" parameter* "|" assign* "]" 
    parameter: parameter_name
    |
    parameter_name: P_ID

    assign: ID ":=" expr "."
    |

    expr: expr_base expr_tail
    expr_base: block 
    | "(" expr ")"
    | ID
    | CLASS_ID 
    | INT 
    | STRING

    expr_tail:  expr_sel 
    expr_sel: S_ID expr_base expr_sel 
    | ID
    |

    ID: /[a-z_][a-zA-Z0-9_]*/
    S_ID: /[a-z_][a-zA-Z0-9_]*:/
    P_ID: /:[a-z_][a-zA-Z0-9_]*/
    CLASS_ID: /[A-Z][a-zA-Z0-9]*/
    INT: /[-+]?[0-9]+/
    STRING: /'(\\'|[^'])*'/

    COMMENT: /"[^"]*"/
    %ignore COMMENT

    %import common.WS
    %ignore WS
    """

    args = sys.argv[1:]
    help_found = False
    for arg in args:
        if re.fullmatch("--help|-h",arg):
            args.remove(arg)
            help_found = True
    if args:
        print("Incorrect argument",file=sys.stderr)
        sys.exit(10)
    elif help_found:
        print("usage: parse.py [-h]\n"

                "Parser for object oriented language SOL25\n"

                "options:]\n"
                "-h, --help  show this help message and exit\n")
        sys.exit(0)

    input_file = sys.stdin.read()
    parser = Lark(grammar, parser='lalr',lexer = 'contextual',lexer_callbacks={'COMMENT': comments.append})
    try:
        tree = parser.parse(text=input_file)
    except (exceptions.UnexpectedCharacters, exceptions.LexError) as e:
        LexicalError()
    except (exceptions.UnexpectedEOF, exceptions.UnexpectedToken) as e:
        SyntaxError()

    kwcheck = KeywordCheck()
    syntax_correct_tree = kwcheck.visit_topdown(tree)

    semantic = SemCreate()
    trans_tree = semantic.transform(syntax_correct_tree)

    xml_tree = to_xml(trans_tree)
    xml_tree.set("language", "SOL25")
    if comments:
        comment = comments[0][1:-1]
        xml_tree.set("description",comment)

    for element in xml_tree.iter():
        attributes_list = []
        for child in element.iter():
            if child is element:
                continue
            elif re.match(rf"{element.tag}_.*", child.tag):
                attributes_list.append(child.tag)
        for attribute in attributes_list:
            
            element.set(attribute.replace(f"{element.tag}_", ""), element.find(f".//{attribute}").text)
            element.remove(element.find(f".//{attribute}"))

    xml_str = ElementTree.tostring(xml_tree, encoding='UTF-8')
    pretty_xml = minidom.parseString(xml_str).toprettyxml(encoding="UTF-8", indent="  ")
    print(pretty_xml.decode("UTF-8")) 
