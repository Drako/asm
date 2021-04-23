#!/usr/bin/env python3
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


def mode_reader(single_column: bool):
    def single_col_reader(contents, columns, inst):
        support = contents[columns['64/32 bit mode support']].text.split('/')
        inst.available_64 = support[0] == 'V'
        inst.available_legacy = support[1] == 'V'

    def multi_col_reader(contents, columns, inst):
        inst.available_64 = contents[columns['64-bit mode']].text == 'Valid'
        inst.available_legacy = contents[columns['compat/leg mode']].text == 'Valid'

    if single_column:
        return single_col_reader
    else:
        return multi_col_reader


def read_instruction_table(table) -> list[Instruction]:
    result = []

    columns = {}
    for idx, tag in enumerate(table.find_all('th')):
        columns[tag.text.lower()] = idx
    if 'opcode' not in columns:
        # TODO: Add support for 'Opcode/Instruction' in single column
        return result

    read_mode = mode_reader('64/32 bit mode support' in columns)

    tbody = table.tbody
    if tbody is None:
        tbody = table

    for row in tbody.find_all('tr')[1:]:
        inst = Instruction()
        contents = row.find_all('td')
        if len(contents) <= 1:
            continue
        inst.opcode = contents[columns['opcode']].text
        inst.instruction = contents[columns['instruction']].text
        if 'op/en' in columns:
            inst.operand_encoding = contents[columns['op/en']].text
        else:
            inst.operand_encoding = 'ZO'
        read_mode(contents, columns, inst)
        inst.description = contents[columns['description']].text
        if inst.available_64:
            # we only want 64 bits instructions
            result.append(inst)

    return result


def generate_spec(instruction: str):
    spec_name = instruction.lower().replace(':', '_').replace('/', '_').replace('.html', '.json')
    response = requests.get(BASE_URI + instruction)
    document = BeautifulSoup(response.text, 'html.parser')
    instructions = read_instruction_table(document.find('h1').find_next_sibling('table'))
    if len(instructions) != 0:
        with open(spec_name, 'w') as spec:
            spec.write(dumps(instructions, cls=InstructionEncoder))


def main():
    all_instructions = get_instructions()
    for cat in SUPPORTED_CATEGORIES:
        for inst in all_instructions[cat]:
            generate_spec(inst)


if __name__ == '__main__':
    main()
