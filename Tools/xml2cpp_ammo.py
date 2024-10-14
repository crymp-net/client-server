#!/usr/bin/env python3

from pathlib import Path
import argparse
import io
import sys
import xml.etree.ElementTree as ET

class AmmoConverter:
	def __init__(self, path: Path, output: io.TextIOWrapper) -> None:
		self.path = path
		self.output = output

	def _write(self, line: str) -> None:
		self.output.write(f'{line}\n')

	def convert(self) -> None:
		self._write('#include "WeaponSystem.h"')
		self._write('')
		self._write('void CWeaponSystem::RegisterXMLData()')
		self._write('{')

		for xml_path in sorted(Path(self.path, 'Scripts/Entities/Items/XML/').glob('**/*.xml')):
			xml_tree = ET.parse(xml_path)
			xml_name = xml_path.relative_to(self.path).as_posix()
			self._process_xml(xml_tree, xml_name)

		self._write('}')

	def _process_xml(self, xml_tree: ET, xml_name: str) -> None:
		root = xml_tree.getroot()

		if root.tag != 'ammo':
			if root.tag != 'item':
				print(f'Ignoring unknown root tag "{root.tag}" in {xml_name}', file=sys.stderr)
			return

		name = root.attrib['name']
		factory = root.attrib['class']
		script = root.attrib['script'] if 'script' in root.attrib else ''
		configuration = root.attrib['configuration'] if 'configuration' in root.attrib else ''

		self._write(f'\t// {xml_name}')
		self._write('\t{')

		self._write('\t\tIItemParamsNode* node = m_pItemSystem->CreateParams();')
		self._process_node(root, 'node')

		self._write('')
		self._write(f'\t\tthis->RegisterAmmo("{name}", "{factory}", "{script}", "{configuration}", node);')
		self._write('\t}')

	def _process_node(self, node: ET.Element, node_name: str, prefix: str = '') -> None:
		for name, value in node.attrib.items():
			self._write(f'{prefix}\t\t{node_name}->SetAttribute("{name}", {self._convert_value(value)});')
		prefix += '\t'
		for child in node:
			self._write(f'{prefix}\t' + '{')
			name = f'{node_name}_{child.tag}' if 'node' in node_name else f'node_{child.tag}'
			self._write(f'{prefix}\t\tIItemParamsNode* {name} = {node_name}->InsertChild("{child.tag}");')
			self._process_node(child, name, prefix)
			self._write(f'{prefix}\t' + '}')

	def _convert_value(self, value: str) -> str:
		# bool
		if value.lower() == 'true':
			return 'true'
		if value.lower() == 'false':
			return 'false'

		# int
		try:
			int(value)
			return f'static_cast<int>({value})'
		except ValueError:
			pass

		# float
		try:
			float(value)
			return f'static_cast<float>({value})'
		except ValueError:
			pass

		# Vec3
		if value.count(',') == 2:
			try:
				x, y, z = value.split(',')
				x = float(x)
				y = float(y)
				z = float(z)
				return f'Vec3({value})'
			except ValueError:
				pass

		# string
		value = value.replace('\\', '/')
		return f'"{value}"'

def dir_path(value: str) -> Path:
	path = Path(value)
	if not path.is_dir():
		raise argparse.ArgumentTypeError(f'"{value}" is not a directory path!')
	return path

def parse_command_line() -> argparse.Namespace:
	parser = argparse.ArgumentParser(description='Crysis ammo XMLs to C++ code convertor')
	parser.add_argument('path', type=dir_path, help='path to extracted GameData.pak and ZPatch1.pak')
	return parser.parse_args()

def main() -> int:
	args = parse_command_line()
	output_file_path = Path(__file__).parent / '..' / 'Code/CryGame/Items/Weapons/WeaponSystem_XMLData.cpp'
	with output_file_path.open(mode='w', encoding='utf-8') as output_file:
		AmmoConverter(args.path, output_file).convert()
	return 0

if __name__ == '__main__':
	sys.exit(main())
