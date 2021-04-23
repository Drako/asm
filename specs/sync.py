from json import JSONEncoder, dumps

import requests
from bs4 import BeautifulSoup

BASE_URI = 'https://www.felixcloutier.com/x86/'

SUPPORTED_CATEGORIES = ['Core Instructions']


class Instruction:
    opcode: str
    instruction: str
    operand_encoding: str
    available_64: bool
    available_legacy: bool
    description: str

    def __init__(self, **kwargs):
        self.opcode = kwargs.get('opcode', '')
        self.instruction = kwargs.get('instruction', '')
        self.operand_encoding = kwargs.get('operand_encoding', '')
        self.available_64 = kwargs.get('available_64', False)
        self.available_legacy = kwargs.get('available_legacy', False)
        self.description = kwargs.get('description', '')

    def __str__(self) -> str:
        return f'(opcode: "{self.opcode}", ' \
               f'instruction: "{self.instruction}", ' \
               f'operand_encoding: "{self.operand_encoding}", ' \
               f'available_64: "{self.available_64}", ' \
               f'available_legacy: "{self.available_legacy}", ' \
               f'description: "{self.description}")'


class InstructionEncoder(JSONEncoder):
    def default(self, obj):
        if isinstance(obj, Instruction):
            return {
                'opcode': obj.opcode,
                'instruction': obj.instruction,
                'operand_encoding': obj.operand_encoding,
                'description': obj.description
            }
        else:
            return JSONEncoder.default(self, obj)


def get_instructions() -> dict[str, list[str]]:
    result = {}
    response = requests.get(BASE_URI)
    document = BeautifulSoup(response.text, 'html.parser')
    for category in document.find_all('h2'):
        table = category.next_sibling
        result[category.text] = sorted(set([inst['href'].replace('./', '') for inst in table.find_all('a')]))
    return result


def mode_reader(columns: dict[str, int]):
    try:
        # it is so nice Intel chose a uniform format for this :|
        combined_columns = [
            '64/32 bit mode support',
            '64/32bit mode support',
            '64/32-bit mode',
            '64/32 -bit mode',
            '64/3 2-bit mode',
            '32/64 bit mode support',
        ]
        combined = next((columns[name] for name in combined_columns if name in columns))

        def single_col_reader(contents, inst):
            support = contents[combined].text.split('/')
            if len(support) == 1:
                support = support[0]
            try:
                inst.available_64 = support[0] == 'V'
                inst.available_legacy = support[1] == 'V'
            except IndexError:
                print(f'{inst.instruction} supported on "{support}"')

        return single_col_reader

    except StopIteration:
        col_64 = columns['64-bit mode']
        col_legacy = columns['compat/leg mode']

        def multi_col_reader(contents, inst):
            inst.available_64 = contents[col_64].text == 'Valid'
            inst.available_legacy = contents[col_legacy].text == 'Valid'

        return multi_col_reader


def op_en_reader(columns: dict[str, int]):
    try:
        col = columns['op/en']

        def reader(contents, inst):
            inst.operand_encoding = contents[col].text

        return reader
    except KeyError:
        def constant(_, inst):
            inst.operand_encoding = 'ZO'

        return constant


def opcode_instruction_reader(columns: dict[str, int]):
    try:
        # it is so nice Intel chose a uniform format for this :|
        combined_columns = [
            'opcode/instruction',
            'opcode / instruction',
            'opcode instruction'
        ]
        combined = next((columns[name] for name in combined_columns if name in columns))

        def single_col_reader(contents, inst):
            opcode_inst = contents[combined].text.split(' ')
            inst.opcode = opcode_inst[0]
            is_instruction = False
            for item in opcode_inst[1:]:
                if len(item) > 2 and item.upper() == item:
                    is_instruction = True
                    inst.instruction = item
                elif is_instruction:
                    inst.instruction += ' ' + item.replace('*', '')
                else:
                    inst.opcode += ' ' + item

        return single_col_reader
    except StopIteration:
        try:
            opcode = columns['opcode']
        except KeyError:
            return None
        instruction = columns['instruction']

        def multi_col_reader(contents, inst):
            inst.opcode = contents[opcode].text
            inst.instruction = contents[instruction].text

        return multi_col_reader


def read_instruction_table(table) -> list[Instruction]:
    result = []

    columns = {}
    for idx, tag in enumerate(table.find_all('th')):
        columns[tag.text.replace('*', '').lower()] = idx

    read_opcode_instruction = opcode_instruction_reader(columns)
    if read_opcode_instruction is None:
        return result
    read_mode = mode_reader(columns)
    read_op_en = op_en_reader(columns)

    tbody = table.tbody
    if tbody is None:
        tbody = table

    for row in tbody.find_all('tr')[1:]:
        inst = Instruction()
        contents = row.find_all('td')
        if len(contents) <= 1:
            continue
        read_opcode_instruction(contents, inst)
        read_op_en(contents, inst)
        read_mode(contents, inst)
        inst.description = contents[columns['description']].text
        if inst.available_64:
            # we only want 64 bits instructions
            result.append(inst)

    return result


def lzcnt() -> list[Instruction]:
    return [
        Instruction(
            opcode='F3 0F BD /r',
            instruction='LZCNT r16, r/m16',
            operand_encoding='RM',
            available_64=True,
            available_legacy=True,
            description='Count the number of leading zero bits in r/m16, return result in r16.'
        ),
        Instruction(
            opcode='F3 0F BD /r',
            instruction='LZCNT r32, r/m32',
            operand_encoding='RM',
            available_64=True,
            available_legacy=False,
            description='Count the number of leading zero bits in r/m32, return result in r32.'
        ),
        Instruction(
            opcode='F3 REX.W 0F BD /r',
            instruction='LZCNT r64, r/m64',
            operand_encoding='RM',
            available_64=True,
            available_legacy=False,
            description='Count the number of leading zero bits in r/m64, return result in r64.'
        )
    ]


def mfence() -> list[Instruction]:
    return [
        Instruction(
            opcode='0F AE 0F',
            instruction='MFENCE',
            operand_encoding='ZO',
            available_64=True,
            available_legacy=True,
            description='Serializes load and store operations.'
        )
    ]


def monitor() -> list[Instruction]:
    return [
        Instruction(
            opcode='0F 01 C8',
            instruction='MONITOR',
            operand_encoding='ZO',
            available_64=True,
            available_legacy=False,
            description='Sets up a linear address range to be monitored by hardware and activates the monitor. The '
                        'address range should be a write-back memory caching type. The address is DS:RAX/EAX/AX.'
        )
    ]


def movbe() -> list[Instruction]:
    return [
        Instruction(
            opcode='0F 38 F0 /r',
            instruction='MOVBE r16, m16',
            operand_encoding='RM',
            available_64=True,
            available_legacy=True,
            description='Reverse byte order in m16 and move to r16.'
        ),
        Instruction(
            opcode='0F 38 F0 /r',
            instruction='MOVBE r32, m32',
            operand_encoding='RM',
            available_64=True,
            available_legacy=True,
            description='Reverse byte order in m32 and move to r32.'
        ),
        Instruction(
            opcode='REX.W + 0F 38 F0 /r',
            instruction='MOVBE r64, m64',
            operand_encoding='RM',
            available_64=True,
            available_legacy=False,
            description='Reverse byte order in m64 and move to r64.'
        ),
        Instruction(
            opcode='0F 38 F1 /r',
            instruction='MOVBE m16, r16',
            operand_encoding='MR',
            available_64=True,
            available_legacy=True,
            description='Reverse byte order in r16 and move to m16.'
        ),
        Instruction(
            opcode='0F 38 F1 /r',
            instruction='MOVBE m32, r32',
            operand_encoding='MR',
            available_64=True,
            available_legacy=True,
            description='Reverse byte order in r32 and move to m32.'
        ),
        Instruction(
            opcode='REX.W + 0F 38 F1 /r',
            instruction='MOVBE m64, r64',
            operand_encoding='MR',
            available_64=True,
            available_legacy=False,
            description='Reverse byte order in r64 and move to m64.'
        ),
    ]


def movdq2q() -> list[Instruction]:
    return [
        Instruction(
            opcode='F2 0F D6 /r',
            instruction='MOVDQ2Q mm, xmm',
            operand_encoding='RM',
            available_64=True,
            available_legacy=True,
            description='Move low quadword from xmm to mmx register.'
        )
    ]


# fuck these
HARDCODED = {
    'LZCNT.html': lzcnt,
    'MFENCE.html': mfence,
    'MONITOR.html': monitor,
    'MOVBE.html': movbe,
    'MOVDQ2Q.html': movdq2q,
}


def generate_spec(instruction: str):
    spec_name = instruction.lower().replace(':', '_').replace('/', '_').replace('.html', '.json')

    if instruction in HARDCODED:
        instructions = HARDCODED[instruction]()
    else:
        response = requests.get(BASE_URI + instruction)
        document = BeautifulSoup(response.text, 'html.parser')
        instructions = read_instruction_table(document.find('h1').find_next_sibling('table'))

    if len(instructions) != 0:
        print(f'{instruction} -> {spec_name}')
        with open(spec_name, 'w') as spec:
            spec.write(dumps(instructions, cls=InstructionEncoder))
    else:
        print(f'{instruction} -> ignored (no 64bit instructions)')


def main():
    all_instructions = get_instructions()
    for cat in SUPPORTED_CATEGORIES:
        for inst in all_instructions[cat]:
            generate_spec(inst)


if __name__ == '__main__':
    main()
