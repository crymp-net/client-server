#!/usr/bin/env python3

from enum import Enum
from pathlib import Path
from typing import Callable
import argparse
import sys
import xml.etree.ElementTree as ET

class SchemaType(Enum):
	TABLE = 1
	ARRAY = 2
	BOOL = 3
	INT = 4
	FLOAT = 5
	STRING = 6
	VEC3 = 7

ROTATION_SCHEMA = {
	'_type': SchemaType.TABLE,
	'_floats': { 'yawSpeed', 'yawAccel', 'pitchSpeed', 'pitchAccel', 'worldSpace' },

	'yawLimits': {
		'_type': SchemaType.ARRAY,

		'yawLimit': {
			'_type': SchemaType.FLOAT,
		}
	},

	'pitchLimits': {
		'_type': SchemaType.ARRAY,

		'pitchLimit': {
			'_type': SchemaType.FLOAT,
		}
	},

	'Sound': {
		'_type': SchemaType.TABLE,
		'_strings': { 'event', 'eventDamage', 'helper', 'param' },
	},

	'RotationTest': {
		'_type': SchemaType.TABLE,
		'_floats': { 'radius' },
		'_strings': { 'helper1', 'helper2' },
	},
}

DAMAGE_MULTIPLIERS_SCHEMA = {
	'_type': SchemaType.ARRAY,

	'DamageMultiplier': {
		'_type': SchemaType.TABLE,
		'_floats': { 'multiplier', 'splash' },
		'_strings': { 'damageType' },
	}
}

DAMAGE_BEHAVIORS_SCHEMA = {
	'_type': SchemaType.ARRAY,

	'DamageBehavior': {
		'_type': SchemaType.TABLE,
		'_bools': { 'ignoreVehicleDestruction' },
		'_floats': { 'damageRatioMin', 'damageRatioMax' },
		'_strings': { 'class' },

		'AISignal': {
			'_type': SchemaType.TABLE,
			'_ints': { 'signalId' },
			'_strings': { 'signalText' },
		},

		'Burn': {
			'_type': SchemaType.TABLE,
			'_floats': { 'damage', 'selfDamage', 'radius', 'interval' },
			'_strings': { 'helper' },
		},

		'CameraShake': {
			'_type': SchemaType.TABLE,
		},

		'CollisionEx': {
			'_type': SchemaType.TABLE,
			'_floats': { 'damages' },
			'_strings': { 'component' },
		},

		'Destroy': {
			'_type': SchemaType.TABLE,
			'_bools': { 'placeholder' },
		},

		'Effect': {
			'_type': SchemaType.TABLE,
			'_bools': { 'disableAfterExplosion' },
			'_strings': { 'effect' },
		},

		'Explosion': {
			'_type': SchemaType.TABLE,
			'_floats': { 'damage', 'radius', 'minRadius', 'minPhysRadius', 'physRadius', 'pressure' },
			'_strings': { 'helper' },
		},

		'Group': {
			'_type': SchemaType.TABLE,
			'_strings': { 'name' },
		},

		'HitPassenger': {
			'_type': SchemaType.TABLE,
			'_ints': { 'damage' },
			'_bools': { 'isDamagePercent' },
		},

		'Impulse': {
			'_type': SchemaType.TABLE,
			'_bools': { 'worldSpace' },
			'_vec3s': { 'direction', 'momentum' },
			'_floats': { 'forceMin', 'forceMax' },
			'_strings': { 'helper' },
		},

		'MovementNotification': {
			'_type': SchemaType.TABLE,
			'_ints': { 'param' },
			# isSteering is defined as float in def_vehicle.xml, but it should be bool for sure
			'_bools': { 'isFatal', 'isDamageAlwaysFull', 'isSteering' },
		},

		'DetachPart': {
			'_type': SchemaType.TABLE,
			'_vec3s': { 'baseForce' },
			'_strings': { 'part' },
		},

		'BlowTire': {
			'_type': SchemaType.TABLE,
		},

		'Indicator': {
			'_type': SchemaType.TABLE,

			'Light': {
				'_type': SchemaType.TABLE,
				'_floats': { 'soundRatioMin' },
				'_strings': { 'material', 'sound', 'helper' },
			}
		},

		'SpawnDebris': {
			'_type': SchemaType.TABLE,
		},
	}
}

WHEELED_SCHEMA = {
	'_type': SchemaType.TABLE,
	'_ints': {
		'axleFriction', 'axleFrictionMax', 'brakeTorque', 'engineIdleRPM', 'engineMaxRPM', 'engineMinRPM',
		'enginePower', 'engineShiftDownRPM', 'engineShiftUpRPM', 'engineStartRPM', 'gearDirSwitchRPM',
		'maxGear',
	},
	'_floats': {
		'brakeImpulse', 'clutchSpeed', 'damping', 'stabilizer', 'stabiMin', 'stabiMax', 'minBrakingFriction',
		'maxBrakingFriction', 'rearWheelBrakingFrictionMult', 'maxSteer', 'maxSpeed', 'maxTimeStep',
		'minEnergy', 'slipThreshold', 'suspDampingMin', 'suspDampingMax', 'suspDampingMaxSpeed',
		'dynFriction', 'steerTrackNeutralTurn', 'pullTilt', 'latFriction',
	},

	'gearRatios': {
		'_type': SchemaType.ARRAY,

		'gearRatio': {
			'_type': SchemaType.FLOAT,
		}
	},
}

SOUND_PARAMS_SCHEMA = {
	'_type': SchemaType.TABLE,
	'_floats': {
		'runSoundDelay', 'rpmPitchSpeed', 'roadBumpMinSusp', 'roadBumpMinSpeed', 'roadBumpIntensity',
		'maxSlipSpeed', 'airbrake',
	},
	'_strings': { 'eventGroup', 'engineSoundPosition' },
}

MOVEMENT_ANIMATIONS_SCHEMA = {
	'_type': SchemaType.TABLE,
	'_strings': { 'engine' },
}

TWEAK_GROUPS_SCHEMA = {
	'_type': SchemaType.ARRAY,

	'TweakGroup': {
		'_type': SchemaType.TABLE,
		'_strings': { 'name' },

		'Tweaks': {
			'_type': SchemaType.ARRAY,

			'Tweak': {
				'_type': SchemaType.TABLE,
				'_ints': { 'op' },
				'_floats': { 'value' },
				'_strings': { 'name' },
			}
		},
	}
}

BOOST_SCHEMA = {
	'_type': SchemaType.TABLE,
	'_floats': { 'endurance', 'regeneration', 'strength' },
}

AIR_DAMP_SCHEMA = {
	'_type': SchemaType.TABLE,
	'_vec3s': { 'dampAngle', 'dampAngVel' },
}

SEAT_ACTION_WEAPONS_SCHEMA = {
	'_type': SchemaType.TABLE,
	'_bools': { 'isSecondary', 'disablesShootToCrosshair' },

	'Weapons': {
		'_type': SchemaType.ARRAY,

		'Weapon': {
			'_type': SchemaType.TABLE,
			'_bools': { 'inheritVelocity' },
			'_strings': { 'class', 'part' },

			'Helpers': {
				'_type': SchemaType.ARRAY,

				'Helper': {
					'_type': SchemaType.STRING,
				}
			},

			'Actions': {
				'_type': SchemaType.ARRAY,

				'Action': {
					'_type': SchemaType.TABLE,
					'_strings': { 'name' },

					'Animations': {
						'_type': SchemaType.ARRAY,

						'Animation': {
							'_type': SchemaType.TABLE,
							'_strings': { 'name' },
						}
					},
				}
			},
		}
	},
}

# def_vehicledamages.xml
DAMAGE_GROUP_SCHEMA = {
	'_type': SchemaType.TABLE,
	'_strings': { 'name', 'useTemplate' },

	'DamagesSubGroups': {
		'_type': SchemaType.ARRAY,

		'DamagesSubGroup': {
			'_type': SchemaType.TABLE,
			'_floats': { 'delay', 'randomness' },

			'DamageBehaviors': {
				'_type': SchemaType.ARRAY,

				'DamageBehavior': {
					'_type': SchemaType.TABLE,
					'_strings': { 'class' },
					'_floats': { 'damageRatioMin', 'damageRatioMax' },
					'_bools': { 'ignoreVehicleDestruction' },

					'AISignal': {
						'_type': SchemaType.TABLE,
						'_ints': { 'signalId' },
						'_strings': { 'signalText' },
					},

					'Burn': {
						'_type': SchemaType.TABLE,
						'_strings': { 'helper' },
						'_floats': { 'damage', 'selfDamage', 'radius', 'interval' },
					},

					'Destroy': {
						'_type': SchemaType.TABLE,
						'_bools': { 'placeholder' },
					},

					'Effect': {
						'_type': SchemaType.TABLE,
						'_strings': { 'effect' },
						'_bools': { 'disableAfterExplosion' },
					},

					'Explosion': {
						'_type': SchemaType.TABLE,
						'_strings': { 'helper' },
						'_floats': { 'damage', 'radius', 'pressure' },
					},

					'Group': {
						'_type': SchemaType.TABLE,
						'_strings': { 'name' },
					},

					'Impulse': {
						'_type': SchemaType.TABLE,
						'_floats': { 'forceMin', 'forceMax' },
						'_vec3s': { 'direction', 'momentum', 'helper' },
					},

					'MovementNotification': {
						'_type': SchemaType.TABLE,
						'_bools': { 'isSteering', 'isFatal' },
					},

					'DetachPart': {
						'_type': SchemaType.TABLE,
						'_strings': { 'part' },
						'_vec3s': { 'baseForce' },
					},

					'BlowTire': {
						'_type': SchemaType.TABLE,
					},

					'Indicator': {
						'_type': SchemaType.TABLE,

						'Light': {
							'_type': SchemaType.TABLE,
							'_strings': { 'material', 'sound', 'helper' },
							'_floats': { 'soundRatioMin' },
						},
					},
				},
			},
		},
	},
}

class VehicleConverter:
	def __init__(self, xml_root: ET.Element, xml_name: str, output_path: Path):
		self.xml_root = xml_root
		self.xml_name = xml_name
		self.output_path = output_path
		self.output_file = None
		self.indentation_level = 0
		self.mods = {}

	def convert_vehicle_implementations(self):
		if self.xml_root.tag != 'Vehicle':
			print(f'{self.xml_name}: Ignoring unknown root tag "{self.xml_root.tag}"', file=sys.stderr)
			return

		vehicle_name = self.xml_root.attrib['name']

		header_file_path = Path(self.output_path, f'{vehicle_name}.h')
		source_file_path = Path(self.output_path, f'{vehicle_name}.cpp')

		with header_file_path.open(mode='w', encoding='utf-8') as h_file:
			self.output_file = h_file
			self._write('#pragma once')
			self._write('')
			self._write('#include "CryAction/Vehicles/Vehicle.h"')
			self._write('')
			self._write(f'class {vehicle_name} : public Vehicle')
			self._write('{')
			self._write('public:')
			self._write(f'\t{vehicle_name}();')
			self._write(f'\t~{vehicle_name}();')
			self._write('')
			self._write('\tbool Init(IGameObject* pGameObject) override;')
			self._write('};')

		with source_file_path.open(mode='w', encoding='utf-8') as cpp_file:
			self.output_file = cpp_file
			self._write(f'#include "{vehicle_name}.h"')
			self._write('')
			self._write(f'{vehicle_name}::{vehicle_name}()')
			self._begin_block()
			self._write(f'// enable {vehicle_name}::Init function')
			self._write('static const bool patched = this->Vehicle::PatchVTable();')
			self._end_block()
			self._write('')
			self._write(f'{vehicle_name}::~{vehicle_name}()')
			self._begin_block()
			self._end_block()
			self._write('')
			self._write(f'bool {vehicle_name}::Init(IGameObject* pGameObject)')
			self._begin_block()
			self._write_vehicle_init()
			self._end_block()

	def convert_damages_templates(self):
		if self.xml_root.tag != 'VehicleDamagesTemplates':
			print(f'{self.xml_name}: Ignoring unknown root tag "{self.xml_root.tag}"', file=sys.stderr)
			return

		output_file_path = Path(self.output_path, f'../VehicleDamagesTemplateRegistry_XMLData.cpp')

		with output_file_path.open(mode='w', encoding='utf-8') as file:
			self.output_file = file
			self._write('#include "VehicleDamagesTemplateRegistry.h"')
			self._write('')
			self._write('void VehicleDamagesTemplateRegistry::InitDefaults()')
			self._begin_block()
			self._write_damages_groups()
			self._end_block()

	def _write_vehicle_init(self):
		self._write('if (!this->Vehicle::Init(pGameObject))')
		self._begin_block()
		self._write('return false;')
		self._end_block()
		self._write('')
		self._write(f'// {self.xml_name}')
		self._write('')

		if 'actionMap' in self.xml_root.attrib:
			action_map = self.xml_root.attrib['actionMap']
			self._write(f'm_actionMapName = "{action_map}";')
			self._write('')

		for modifications in self.xml_root.findall('./Modifications'):
			self._load_mods(modifications)

		for physics in self.xml_root.findall('./Physics'):
			self._write('// Physics')
			self._process_physics(physics)
			self._write('')

		self._write('Vehicle::ComponentParts componentParts;')
		self._write('')

		for parts in self.xml_root.findall('./Parts'):
			self._write('// Parts')
			self._process_parts(parts)
			self._write('')

		self._write('// Helpers')
		self._write('this->InitHelpers();')
		self._write('')

		for components in self.xml_root.findall('./Components'):
			self._write('// Components')
			self._process_components(components)
			self._write('')
			self._write('this->RegisterComponentParts(componentParts);')
			self._write('')

		for particles in self.xml_root.findall('./Particles'):
			self._write('// Particles')
			self._process_particles(particles)
			self._write('')

		for animations in self.xml_root.findall('./Animations'):
			self._write('// Animations')
			self._process_animations(animations)
			self._write('')

		for movement_params in self.xml_root.findall('./MovementParams'):
			self._write('// MovementParams')
			self._process_movement_params(movement_params)
			self._write('')

		self._write('SmartScriptTable seats(gEnv->pScriptSystem);')
		self._write('')

		for seats in self.xml_root.findall('./Seats'):
			self._write('// Seats')
			self._process_seats(seats)
			self._write('')

		self._write('IScriptTable* pScriptTable = this->GetEntity()->GetScriptTable();')
		self._write('if (pScriptTable)')
		self._begin_block()
		self._write('pScriptTable->SetValue("Seats", seats);')
		self._end_block()
		self._write('')

		self._write('// SeatTransitions')
		self._write('this->InitSeatTransitions();')
		self._write('')

		for seat_groups in self.xml_root.findall('./SeatGroups'):
			self._write('// SeatGroups')
			self._process_seat_groups(seat_groups)
			self._write('')

		for inventory in self.xml_root.findall('./Inventory'):
			self._write('// Inventory')
			self._process_inventory(inventory)
			self._write('')

		for paints in self.xml_root.findall('./Paints'):
			self._write('// Paints')
			self._process_paints(paints)
			self._write('')

		for actions in self.xml_root.findall('./Actions'):
			self._write('// Actions')
			self._process_actions(actions)
			self._write('')

		for damages in self.xml_root.findall('./Damages'):
			self._write('// Damages')
			self._process_damages(damages)
			self._write('')

		# TODO: dump animated parts when v_debugdraw cvar is set to 3

		self._write('this->InitMaxDamage();')
		self._write('this->AttachScriptBindToSeats();')
		self._write('')

		self._write('return this->BindVehicleToNetwork();')

	def _write_damages_groups(self):
		self._write(f'// {self.xml_name}')
		self._write('')
		for groups in self.xml_root.findall('./DamagesGroups'):
			for group in groups.findall('./DamagesGroup'):
				self._process_damages_group(group)

	def _write(self, line: str):
		line = ('\t' * self.indentation_level) + line if line else ''
		print(line, file=self.output_file)

	def _begin_block(self):
		self._write('{')
		self.indentation_level += 1

	def _end_block(self):
		self.indentation_level -= 1
		self._write('}')

	def _convert_bool(self, value: str) -> str:
		match value:
			case '0':
				return 'false'
			case '1':
				return 'true'
			case _:
				raise ValueError(f'Invalid bool attribute value "{value}"')

	def _convert_int(self, value: str) -> str:
		return f'{value}'

	def _convert_float(self, value: str) -> str:
		return f'{value}f' if '.' in value else f'{value}.0f'

	def _convert_string(self, value: str) -> str:
		return f'"{value}"'

	def _convert_vec3(self, value: str) -> str:
		x, y, z = map(str.strip, value.split(','))
		return f'Vec3({x}, {y}, {z})'

	def _lower_first_letter(self, string: str) -> str:
		return string[0].lower() + string[1:]

	def _get_mods(self, element: ET.Element, name: str) -> dict[str, str]:
		if 'id' in element.attrib:
			ref = element.attrib['id']
			if ref in self.mods and name in self.mods[ref]:
				return self.mods[ref][name]
		return {}

	def _write_value(self, element: ET.Element, name: str, prefix: str, suffix: str, convert: Callable[[str], str]):
		default_value = element.attrib[name]
		mods = self._get_mods(element, name)
		if mods:
			statement = 'if'
			for mod_name, mod_value in mods.items():
				check = 'StringTools::IsEqualNoCase'
				self._write(f'{statement} ({check}(m_modName, "{mod_name}"))')
				self._begin_block()
				self._write(f'{prefix}{convert(mod_value)}{suffix}')
				self._end_block()
				statement = 'else if'
			self._write('else')
			self._begin_block()
			self._write(f'{prefix}{convert(default_value)}{suffix}')
			self._end_block()
		else:
			self._write(f'{prefix}{convert(default_value)}{suffix}')

	def _write_table(self, element: ET.Element, schema: dict, table_name: str):
		assert schema['_type'] in (SchemaType.TABLE, SchemaType.ARRAY)

		self._write(f'SmartScriptTable {table_name}(gEnv->pScriptSystem);')

		for name, value in element.attrib.items():
			prefix = f'{table_name}->SetValue("{name}", '
			suffix = ');'
			if name == 'id':
				pass
			elif '_ints' in schema and name in schema['_ints']:
				self._write_value(element, name, prefix, suffix, self._convert_int)
			elif '_bools' in schema and name in schema['_bools']:
				self._write_value(element, name, prefix, suffix, self._convert_bool)
			elif '_vec3s' in schema and name in schema['_vec3s']:
				self._write_value(element, name, prefix, suffix, self._convert_vec3)
			elif '_floats' in schema and name in schema['_floats']:
				self._write_value(element, name, prefix, suffix, self._convert_float)
			elif '_strings' in schema and name in schema['_strings']:
				self._write_value(element, name, prefix, suffix, self._convert_string)
			else:
				self._write(f'// Unknown attribute {name}="{value}"')

		for child in element:
			if child.tag not in schema:
				self._write(f'// Unknown element "{child.tag}"')
				continue

			child_type = schema[child.tag]['_type']

			is_array = schema['_type'] == SchemaType.ARRAY
			is_new_block = is_array and child_type in (SchemaType.TABLE, SchemaType.ARRAY)

			if is_new_block:
				self._begin_block()

			if is_array:
				prefix = f'{table_name}->PushBack('
				suffix = ');'
			else:
				prefix = f'{table_name}->SetValue("{child.tag}", '
				suffix = ');'

			if child_type in (SchemaType.TABLE, SchemaType.ARRAY):
				child_table_name = child.tag.lower()
				if 'name' in child.attrib:
					child_table_name = child.attrib['name'] + '_' + child_table_name
				if not is_array:
					child_table_name = table_name + '_' + child_table_name

				self._write_table(child, schema[child.tag], child_table_name)

				self._write(f'{prefix}{child_table_name}{suffix}')
			elif child_type == SchemaType.INT:
				self._write_value(child, 'value', prefix, suffix, self._convert_int)
			elif child_type == SchemaType.BOOL:
				self._write_value(child, 'value', prefix, suffix, self._convert_bool)
			elif child_type == SchemaType.VEC3:
				self._write_value(child, 'value', prefix, suffix, self._convert_vec3)
			elif child_type == SchemaType.FLOAT:
				self._write_value(child, 'value', prefix, suffix, self._convert_float)
			elif child_type == SchemaType.STRING:
				self._write_value(child, 'value', prefix, suffix, self._convert_string)
			else:
				raise ValueError(f'Invalid schema _type "{child_type}"')

			if is_new_block:
				self._end_block()

	def _load_mods(self, mods: ET.Element):
		for mod in mods:
			mod_name = mod.attrib['name']
			if 'parent' in mod.attrib:
				parent_name = mod.attrib['parent']
				for parent in mods.findall(f'./Modification[@name="{parent_name}"]'):
					assert 'parent' not in parent.attrib
					self._add_mod(parent, mod_name)
			self._add_mod(mod, mod_name)

	def _add_mod(self, mod: ET.Element, mod_name: str):
		assert mod.tag == 'Modification'
		for elems in mod:
			assert elems.tag == 'Elems'
			for elem in elems:
				assert elem.tag == 'Elem'
				elem_ref = elem.attrib['idRef']
				elem_name = elem.attrib['name']
				elem_value = elem.attrib['value']
				if elem_ref not in self.mods:
					self.mods[elem_ref] = {}
				if elem_name not in self.mods[elem_ref]:
					self.mods[elem_ref][elem_name] = {}
				self.mods[elem_ref][elem_name][mod_name] = elem_value

	################################################################################
	# Physics
	################################################################################

	def _process_physics(self, physics: ET.Element):
		for name, value in physics.attrib.items():
			match name:
				case 'id':
					pass
				case 'damping':
					self._write_value(physics, name, 'm_physics.damping = ', ';', self._convert_float)
				case 'dampingFreefall':
					self._write_value(physics, name, 'm_physics.dampingFreefall = ', ';', self._convert_float)
				case 'gravityFreefall':
					self._write_value(physics, name, 'm_physics.gravityFreefall = ', ';', self._convert_vec3)
				case 'retainGravity':
					self._write_value(physics, name, 'm_retainGravity = ', ';', self._convert_bool)
				case 'pushable':
					if self._convert_bool(value) == 'false':
						self._write('m_physicsParams.flagsAND = ~pef_pushable_by_players;')
				case _:
					self._write(f'// Unknown attribute {name}="{value}"')
		for child in physics:
			match child.tag:
				case 'Buoyancy':
					self._process_physics_buoyancy(child)
				case 'Simulation':
					self._process_physics_simulation(child)
				case _:
					self._write(f'// Unknown element "{child.tag}"')

	def _process_physics_buoyancy(self, buoyancy: ET.Element):
		for name, value in buoyancy.attrib.items():
			match name:
				case 'id':
					pass
				case 'waterDamping':
					self._write_value(buoyancy, name, 'm_buoyancy.waterDamping = ', ';', self._convert_float)
				case 'waterDensity':
					self._write_value(buoyancy, name, 'm_buoyancy.waterDensity = ', ';', self._convert_float)
				case 'waterResistance':
					self._write_value(buoyancy, name, 'm_buoyancy.waterResistance = ', ';', self._convert_float)
				case _:
					self._write(f'// Unknown attribute {name}="{value}"')
		for child in buoyancy:
			self._write(f'// Unknown element "{child.tag}"')

	def _process_physics_simulation(self, simulation: ET.Element):
		for name, value in simulation.attrib.items():
			match name:
				case 'id':
					pass
				case 'maxTimeStep':
					# TODO: limit to 0.04 if vehicle quality is 1
					self._write_value(simulation, name, 'm_physics.maxTimeStep = ', ';', self._convert_float)
				case 'minEnergy':
					self._write_value(simulation, name, 'm_physics.minEnergy = ', ';', self._convert_float)
				case 'maxLoggedCollisions':
					self._write_value(simulation, name, 'm_physics.maxLoggedCollisions = ', ';', self._convert_int)
				case _:
					self._write(f'// Unknown attribute {name}="{value}"')
		for child in simulation:
			self._write(f'// Unknown element "{child.tag}"')

	################################################################################
	# Parts
	################################################################################

	def _get_parts_schema(self):
		parts_schema = {
			'_type': SchemaType.ARRAY,

			'Part': {
				'_type': SchemaType.TABLE,
				'_ints': { 'useOption' },
				'_bools': { 'disablePhysics', 'disableCollision', 'isHidden' },
				'_vec3s': { 'position' },
				'_floats': { 'mass' },
				'_strings': { 'name', 'class', 'component', 'helper', 'filename' },

				'Components': {
					'_type': SchemaType.ARRAY,

					'Component': {
						'_type': SchemaType.STRING,
					}
				},

				'Helpers': {
					'_type': SchemaType.ARRAY,

					'Helper': {
						'_type': SchemaType.TABLE,
						'_vec3s': { 'position', 'direction' },
						'_strings': { 'name' },
					}
				},

				'Animated': {
					'_type': SchemaType.TABLE,
					'_strings': { 'filename', 'filenameDestroyed', 'destroyedSuffix' },

					'Rotation': ROTATION_SCHEMA,
				},

				'AnimatedJoint': {
					'_type': SchemaType.TABLE,
					'_vec3s': { 'detachBaseForce' },
					'_floats': { 'detachProbability' },
					'_strings': { 'filename', 'filenameDestroyed' },

					'Rotation': ROTATION_SCHEMA,

					'Dials': {
						'_type': SchemaType.TABLE,
						'_floats': { 'rotationMax', 'ofs' },
					},
				},

				'MassBox': {
					'_type': SchemaType.TABLE,
					'_bools': { 'lumberjack' },
					'_vec3s': { 'size' },
					'_floats': { 'drivingOffset' },
				},

				'Static': {
					'_type': SchemaType.TABLE,
					'_strings': { 'filename', 'filenameDestroyed', 'geometry' },

					'Rotation': ROTATION_SCHEMA,
				},

				'SubPart': {
					'_type': SchemaType.TABLE,

					'Rotation': ROTATION_SCHEMA,
				},

				'SubPartWheel': {
					'_type': SchemaType.TABLE,
					'_ints': { 'axle', 'surfaceId' },
					'_bools': { 'canBrake', 'driving', 'rayCast' },
					'_floats': {
						'density', 'damping', 'lenInit', 'lenMax', 'maxFriction',
						'minFriction', 'slipFrictionMod', 'slipSlope', 'stiffness',
						'suspLength', 'rimRadius', 'torqueScale'
					},
				},

				'Tread': {
					'_type': SchemaType.TABLE,
					'_floats': { 'uvRateMax', 'uvSliceCount' },
					'_strings': { 'filename', 'materialName' },
				},

				'Light': {
					'_type': SchemaType.TABLE,
					'_ints': { 'style' },
					'_bools': { 'fakeLight' },
					'_vec3s': { 'diffuse', 'specular' },
					'_floats': {
						'radius', 'diffuseMult', 'specularMult', 'coronaDistIntensity',
						'coronaDistSize', 'coronaScale', 'directFactor', 'frustumAngle',
						'frequency', 'duration'
					},
					'_strings': { 'type', 'texture', 'material' },
				},
			},
		}

		parts_schema['Part']['Parts'] = parts_schema

		return parts_schema

	def _process_parts(self, parts: ET.Element):
		self._begin_block()
		self._write_table(parts, self._get_parts_schema(), 'parts')
		self._write('')
		self._write('this->LoadParts(parts, nullptr, componentParts);')
		self._write('')
		self._write('for (auto& [name, pPart] : m_parts)')
		self._begin_block()
		self._write('pPart->ChangeState(IVehiclePart::eVGS_Default);')
		self._end_block()
		self._end_block()

	################################################################################
	# Components
	################################################################################

	def _process_components(self, components: ET.Element):
		component_schema = {
			'_type': SchemaType.TABLE,
			'_bools': { 'major', 'useBoundsFromParts', 'useDamageLevels', 'isOnlyDamageableByPlayer' },
			'_vec3s': { 'minBound', 'maxBound' },
			'_floats': { 'damageMax', 'hullAffection' },
			'_strings': { 'name' },

			'DamageMultipliers': DAMAGE_MULTIPLIERS_SCHEMA,
			'DamageBehaviors': DAMAGE_BEHAVIORS_SCHEMA,
		}

		for name, value in components.attrib.items():
			self._write(f'// Unknown attribute {name}="{value}"')

		for component in components:
			if component.tag != 'Component':
				self._write(f'// Unknown element "{component.tag}"')
				continue

			self._begin_block()
			self._write_table(component, component_schema, 'component')
			self._write('')
			component_name = self._lower_first_letter(component.attrib['name'])
			self._write(f'VehicleComponent* {component_name} = new VehicleComponent();')
			self._write(f'{component_name}->Init(this, component);')
			self._write(f'm_components.push_back({component_name});')
			self._end_block()

	################################################################################
	# Particles
	################################################################################

	def _process_particles(self, particles: ET.Element):
		particles_schema = {
			'_type': SchemaType.TABLE,

			'Exhaust': {
				'_type': SchemaType.TABLE,
				'_bools': { 'insideWater', 'outsideWater' },

				'Helpers': {
					'_type': SchemaType.ARRAY,

					'Helper': {
						'_type': SchemaType.STRING,
					}
				},

				'EngineStart': {
					'_type': SchemaType.TABLE,
					'_strings': { 'effect' },
				},

				'EngineStop': {
					'_type': SchemaType.TABLE,
					'_strings': { 'effect' },
				},

				'EngineRunning': {
					'_type': SchemaType.TABLE,
					'_bools': { 'disableWithNegativePower' },
					'_floats': {
						'baseSizeScale',
						'minSpeed',
						'minSpeedSizeScale',
						'minSpeedCountScale',
						'minSpeedSpeedScale',
						'maxSpeed',
						'maxSpeedSizeScale',
						'maxSpeedCountScale',
						'maxSpeedSpeedScale',
						'minPower',
						'minPowerSizeScale',
						'minPowerCountScale',
						'minPowerSpeedScale',
						'maxPower',
						'maxPowerSizeScale',
						'maxPowerCountScale',
						'maxPowerSpeedScale',
					},
					'_strings': { 'effect', 'boostEffect' },
				},
			},

			'ShatterEffect': {
				'_type': SchemaType.TABLE,
				'_strings': { 'effect' },
			},

			'DamageEffects': {
				'_type': SchemaType.ARRAY,

				'DamageEffect': {
					'_type': SchemaType.TABLE,
					'_vec3s': { 'gravityDirection' },
					'_floats': { 'scaleMax', 'pulsePeriod' },
					'_strings': { 'name', 'helper', 'effect' },
				}
			},

			'EnvironmentLayers': {
				'_type': SchemaType.ARRAY,

				'EnvironmentLayer': {
					'_type': SchemaType.TABLE,
					'_bools': { 'active' },
					'_floats': {
						'minSpeed',
						'minSpeedSizeScale',
						'minSpeedCountScale',
						'minSpeedSpeedScale',
						'maxSpeed',
						'maxSpeedSizeScale',
						'maxSpeedCountScale',
						'maxSpeedSpeedScale',
						'minPowerSizeScale',
						'minPowerCountScale',
						'minPowerSpeedScale',
						'maxPowerSizeScale',
						'maxPowerCountScale',
						'maxPowerSpeedScale',
					},
					'_strings': { 'name' },

					'Alignment': {
						'_type': SchemaType.TABLE,
						'_bools': { 'alignToWater' },
						'_floats': {
							'alignGroundHeight',
							'maxHeightSizeScale',
							'maxHeightCountScale',
						},
					},

					'Emitters': {
						'_type': SchemaType.ARRAY,

						'Helper': {
							'_type': SchemaType.STRING,
						}
					},

					'Splashes': {
						'_type': SchemaType.TABLE,
						'_strings': { 'effect' },
					},

					'Wheels': {
						'_type': SchemaType.TABLE,
						'_bools': { 'all', 'allActive' },

						'WheelGroups': {
							'_type': SchemaType.ARRAY,

							'WheelGroup': {
								'_type': SchemaType.ARRAY,

								'Wheels': {
									'_type': SchemaType.ARRAY,

									'Wheel': {
										'_type': SchemaType.INT,
									}
								}
							}
						},
					},
				}
			},
		}

		self._begin_block()
		self._write_table(particles, particles_schema, 'particles')
		self._write('')
		self._write('SmartScriptTable table(gEnv->pScriptSystem);')
		self._write('table->SetValue("Particles", particles);')
		self._write('this->InitParticles(table);')
		self._end_block()

	################################################################################
	# Animations
	################################################################################

	def _process_animations(self, animations: ET.Element):
		animation_schema = {
			'_type': SchemaType.TABLE,
			'_strings': { 'name', 'part' },

			'States': {
				'_type': SchemaType.ARRAY,

				'State': {
					'_type': SchemaType.TABLE,
					'_bools': { 'isLooped', 'isLoopedEx' },
					'_floats': { 'speedDefault', 'speedMin', 'speedMax' },
					'_strings': { 'name', 'animation', 'sound', 'soundHelper' },

					'Materials': {
						'_type': SchemaType.ARRAY,

						'Material': {
							'_type': SchemaType.TABLE,
							'_bools': { 'invertValue' },
							'_strings': { 'name', 'setting' },
						}
					},
				}
			},
		}

		for name, value in animations.attrib.items():
			self._write(f'// Unknown attribute {name}="{value}"')

		for animation in animations:
			if animation.tag != 'Animation':
				self._write(f'// Unknown element "{animation.tag}"')
				continue

			self._begin_block()
			self._write_table(animation, animation_schema, 'animation')
			self._write('')
			animation_name = animation.attrib['name']
			self._write(f'VehicleAnimation* {animation_name} = new VehicleAnimation();')
			self._write(f'{animation_name}->Init(this, animation);')
			self._write(f'm_animations.push_back({{ "{animation_name}", {animation_name} }});')
			self._end_block()

	################################################################################
	# MovementParams
	################################################################################

	def _process_movement_params(self, movements: ET.Element):
		std_wheeled_schema = {
			'_type': SchemaType.TABLE,
			'_bools': { 'isBreakingOnIdle' },
			'_floats': {
				'steerMax', 'vMaxSteerMax', 'steerSpeed', 'steerSpeedMin', 'steerSpeedScale',
				'steerSpeedScaleMin', 'v0SteerMax', 'kvSteerMax', 'steerRelaxation', 'pedalLimitMax',
				'engineIgnitionTime', 'rpmInterpSpeed', 'rpmRelaxSpeed', 'rpmGearShiftSpeed',
			},

			'Wheeled': WHEELED_SCHEMA,
			'SoundParams': SOUND_PARAMS_SCHEMA,
			'Animations': MOVEMENT_ANIMATIONS_SCHEMA,
			'TweakGroups': TWEAK_GROUPS_SCHEMA,
			'Boost': BOOST_SCHEMA,
			'AirDamp': AIR_DAMP_SCHEMA,
		}
		std_boat_schema = {
			'_type': SchemaType.TABLE,
			'_vec3s': { 'waveIdleStrength' },
			'_floats': {
				'velMax', 'velMaxReverse', 'velLift', 'liftDensity', 'pedalLimitReverse',
				'acceleration', 'accelerationVelMax', 'accelerationMultiplier', 'pushTilt',
				'turnRateMax', 'turnAccel', 'turnVelocityMult', 'turnAccelMultiplier', 'turnDamping',
				'cornerForce', 'cornerTilt',
			},
			'_strings': { 'pushHelper', 'cornerHelper' },

			'SoundParams': SOUND_PARAMS_SCHEMA,
			'Animations': MOVEMENT_ANIMATIONS_SCHEMA,
			'TweakGroups': TWEAK_GROUPS_SCHEMA,
			'Boost': BOOST_SCHEMA,
			'AirDamp': AIR_DAMP_SCHEMA,
		}
		hovercraft_schema = {
			'_type': SchemaType.TABLE,
			'_ints': { 'numThrusters' },
			'_bools': { 'sampleByHelpers', 'bEngineAlwaysOn' },
			'_vec3s': { 'pushOffset', 'cornerOffset' },
			'_floats': {
				'hoverHeight', 'hoverVariance', 'hoverFrequency', 'thrusterBottomOffset',
				'thrusterMaxHeightCoeff', 'stiffness', 'damping', 'dampingLimit', 'thrusterTilt',
				'velMax', 'velMaxReverse', 'acceleration', 'accelerationMultiplier', 'pushTilt',
				'linearDamping', 'turnRateMax', 'turnRateReverse', 'turnAccel', 'turnAccelMultiplier',
				'turnDamping', 'cornerForce', 'cornerTilt', 'thrusterHeightAdaption', 'rayLength',
				'thrusterUpdate',
			},

			'Thrusters': {
				'_type': SchemaType.ARRAY,

				'Thruster': {
					'_type': SchemaType.TABLE,
					'_bools': { 'pushing' },
					'_floats': { 'hoverVariance', 'heightAdaption', 'hoverHeight', 'cylinder' },
					'_strings': { 'helper' },
				}
			},

			'SoundParams': SOUND_PARAMS_SCHEMA,
			'Animations': MOVEMENT_ANIMATIONS_SCHEMA,
			'TweakGroups': TWEAK_GROUPS_SCHEMA,
			'Boost': BOOST_SCHEMA,
			'AirDamp': AIR_DAMP_SCHEMA,
		}
		warrior_schema = {
			'_type': SchemaType.TABLE,
			'_ints': { 'maxThrustersDamaged', 'recoveryTime' },
			'_floats': { 'collapsedLegAngle', 'collapsedFeetAngle' },

			'Hovercraft': hovercraft_schema,
			'SoundParams': SOUND_PARAMS_SCHEMA,
			'Animations': MOVEMENT_ANIMATIONS_SCHEMA,
		}
		helicopter_schema = {
			'_type': SchemaType.TABLE,
			'_floats': {
				'engineWarmupDelay', 'engineIgnitionTime', 'enginePowerMax', 'rotationDamping',
				'altitudeMax', 'rotorDiskTiltScale', 'pitchResponsiveness', 'rollResponsiveness',
				'yawResponsiveness', 'maxYawRate', 'maxFwdSpeed', 'maxLeftSpeed', 'maxSpeed',
				'maxUpSpeed', 'basicSpeedFraction', 'yawDecreaseWithSpeed', 'tiltPerVelDifference',
				'maxTiltAngle', 'maxRollAngle', 'extraRollForTurn', 'rollForTurnForce', 'yawPerRoll',
				'pitchActionPerTilt', 'pitchInputConst', 'powerInputConst', 'powerInputDamping',
				'relaxForce', 'yawInputConst', 'yawInputDamping',
			},

			'SoundParams': SOUND_PARAMS_SCHEMA,
			'Animations': MOVEMENT_ANIMATIONS_SCHEMA,
			'TweakGroups': TWEAK_GROUPS_SCHEMA,
			'Boost': BOOST_SCHEMA,
		}
		tank_schema = {
			'_type': SchemaType.TABLE,
			'_floats': {
				'pedalSpeed', 'pedalThreshold', 'steerSpeed', 'steerSpeedRelax', 'steerLimit',
				'latFricMin', 'latFricMinSteer', 'latFricMax', 'latSlipMin', 'latSlipMax',
				'rpmInterpSpeed', 'rpmRelaxSpeed', 'rpmGearShiftSpeed', 'steeringImpulseMin',
				'steeringImpulseMax', 'steeringImpulseRelaxMin', 'steeringImpulseRelaxMax',
			},

			'Wheeled': WHEELED_SCHEMA,
			'SoundParams': SOUND_PARAMS_SCHEMA,
			'Animations': MOVEMENT_ANIMATIONS_SCHEMA,
			'TweakGroups': TWEAK_GROUPS_SCHEMA,
			'Boost': BOOST_SCHEMA,
			'AirDamp': AIR_DAMP_SCHEMA,
		}
		amphibious_schema = {
			'_type': SchemaType.TABLE,

			'StdWheeled': std_wheeled_schema,
			'StdBoat': std_boat_schema,
		}
		vtol_schema = {
			'_type': SchemaType.TABLE,
			'_floats': {
				'engineWarmupDelay', 'engineIgnitionTime', 'enginePowerMax', 'rotationDamping',
				'angleLift', 'altitudeMax', 'rotorDiskTiltScale', 'pitchResponsiveness',
				'rollResponsiveness', 'yawResponsiveness', 'maxYawRate', 'maxFwdSpeed',
				'maxLeftSpeed', 'maxSpeed', 'maxUpSpeed', 'basicSpeedFraction',
				'yawDecreaseWithSpeed', 'tiltPerVelDifference', 'maxTiltAngle', 'extraRollForTurn',
				'rollForTurnForce', 'yawPerRoll', 'pitchActionPerTilt', 'powerInputConst',
				'powerInputDamping', 'pitchInputConst', 'yawInputConst', 'yawInputDamping',
				'horizFwdForce', 'horizLeftForce', 'boostForce', 'strafeForce', 'relaxForce',
				'maxRollAngle', 'wingsSpeed',
			},
			'_strings': { 'impulseHelper', 'WingComponentLeft', 'WingComponentRight' },

			'SoundParams': SOUND_PARAMS_SCHEMA,
			'Animations': MOVEMENT_ANIMATIONS_SCHEMA,
			'TweakGroups': TWEAK_GROUPS_SCHEMA,
			'Boost': BOOST_SCHEMA,
		}

		schemas = {
			'StdWheeled': std_wheeled_schema,
			'StdBoat': std_boat_schema,
			'Hovercraft': hovercraft_schema,
			'Warrior': warrior_schema,
			'Helicopter': helicopter_schema,
			'Tank': tank_schema,
			'Amphibious': amphibious_schema,
			'VTOL': vtol_schema,
		}

		for name, value in movements.attrib.items():
			self._write(f'// Unknown attribute {name}="{value}"')

		for movement in movements:
			if movement.tag not in schemas:
				self._write(f'// Unknown vehicle movement "{movement.tag}"')
				continue

			self._begin_block()
			self._write_table(movement, schemas[movement.tag], 'movement')
			self._write('')
			self._write(f'if (!this->SetMovement("{movement.tag}", movement))')
			self._begin_block()
			self._write('return false;')
			self._end_block()
			self._end_block()

	################################################################################
	# Seats
	################################################################################

	def _process_seats(self, seats: ET.Element):
		seat_schema = {
			'_type': SchemaType.TABLE,
			'_ints': { 'seatGroupIndex', 'agSeatNumber' },
			'_bools': {
				'movePassengerOnExit', 'useBoundsForEntering', 'isDriver', 'isPassengerShielded',
				'isPassengerHidden', 'ragdollOnDeath', 'isPassengerExposed',
				'disableStopAllAnimationsOnEnter', 'transitionAnim', 'locked',
			},
			'_vec3s': { 'exitOffsetPlayer' },
			'_floats': { 'blurMult', 'explosionShakeMult' },
			'_strings': {
				'name', 'part', 'enterHelper', 'exitHelper', 'sitHelper', 'AimPart',
				'usesSeatForEntering', 'agVehicleName', 'remotelyUseActionsFromSeat',
			},

			'Views': {
				'_type': SchemaType.ARRAY,

				'View': {
					'_type': SchemaType.TABLE,
					'_bools': { 'canRotate', 'hidePlayer', 'isAvailableRemotely' },
					'_vec3s': { 'rotationMin', 'rotationMax', 'rotationInit' },
					'_floats': {
						'rotationBoundsActionMult', 'delayTimeMax', 'relaxTimeMax',
						'rotationVelMin', 'rotationVelMax',
					},
					'_strings': { 'class' },

					'HideParts': {
						'_type': SchemaType.ARRAY,

						'HidePart': {
							'_type': SchemaType.STRING,
						}
					},

					'FirstPerson': {
						'_type': SchemaType.TABLE,
						'_bools': { 'hidePlayer', 'hideVehicle' },
						'_vec3s': { 'offset' },
						'_floats': { 'relativeToHorizon' },
						'_strings': { 'helper', 'frameObject' },
					},

					'ThirdPerson': {
						'_type': SchemaType.TABLE,
						'_floats': { 'distance', 'heightOffset', 'speed' },
					},

					'ActionThirdPerson': {
						'_type': SchemaType.TABLE,
						'_vec3s': { 'cameraPosOffset', 'cameraAimOffset', 'velocityMult' },
						'_floats': { 'heightOffset', 'lagSpeed', 'heightAboveWater' },
					},
				}
			},

			'SeatActions': {
				'_type': SchemaType.ARRAY,

				'SeatAction': {
					'_type': SchemaType.TABLE,
					'_bools': { 'isAvailableRemotely' },
					'_strings': { 'class' },

					'Lights': {
						'_type': SchemaType.TABLE,
						'_ints': { 'sound' },
						'_strings': { 'activation' },

						'LightParts': {
							'_type': SchemaType.ARRAY,

							'LightPart': {
								'_type': SchemaType.STRING,
							}
						}
					},

					'PassengerIK': {
						'_type': SchemaType.TABLE,
						'_floats': { 'waitShortlyBeforeStarting' },

						'Limbs': {
							'_type': SchemaType.ARRAY,

							'Limb': {
								'_type': SchemaType.TABLE,
								'_strings': { 'limb', 'helper' },
							}
						},
					},

					'RotateTurret': {
						'_type': SchemaType.TABLE,
						'_strings': { 'pitchPart', 'yawPart' },
					},

					'Sound': {
						'_type': SchemaType.TABLE,
						'_strings': { 'sound', 'helper' },
					},

					'Animation': {
						'_type': SchemaType.TABLE,
						'_bools': { 'manualUpdate' },
						'_floats': { 'speed' },
						'_strings': { 'vehicleAnimation', 'control', 'sound', 'stopSound' },
					},

					'SteeringWheel': {
						'_type': SchemaType.TABLE,
						'_strings': { 'vehicleAnimation' },

						'Car': {
							'_type': SchemaType.TABLE,
							'_floats': { 'wheelRotationMax' },
						},

						'Actions': {
							'_type': SchemaType.TABLE,
							'_vec3s': { 'anglesMax' },
							'_floats': { 'steeringForce', 'steeringRelaxMult' },
						},
					},

					'Weapons': SEAT_ACTION_WEAPONS_SCHEMA,
				}
			},

			'Transitions': {
				'_type': SchemaType.TABLE,

				'WaitFor': {
					'_type': SchemaType.ARRAY,

					'Seat': {
						'_type': SchemaType.STRING,
					}
				},
			},

			'Animations': {
				'_type': SchemaType.TABLE,
				'_strings': { 'idle', 'death', 'enter', 'exit' },
			},

			'Sounds': {
				'_type': SchemaType.TABLE,
				'_floats': { 'inout', 'mood' },
				'_strings': { 'enter', 'exit' },
			},
		}

		for name, value in seats.attrib.items():
			self._write(f'// Unknown attribute {name}="{value}"')

		for seat in seats:
			if seat.tag != 'Seat':
				self._write(f'// Unknown element "{seat.tag}"')
				continue

			self._begin_block()
			self._write_table(seat, seat_schema, 'seat')
			self._write('seats->PushBack(seat);')
			self._write('')
			seat_name = seat.attrib['name']
			self._write(f'VehicleSeat* {seat_name} = new VehicleSeat();')
			self._write(f'{seat_name}->Init(this, static_cast<TVehicleSeatId>(m_seats.size() + 1), seat);')
			self._write(f'm_seats.push_back({{ "{seat_name}", {seat_name} }});')
			self._end_block()

	################################################################################
	# SeatGroups
	################################################################################

	def _process_seat_groups(self, seat_groups: ET.Element):
		seat_group_schema = {
			'_type': SchemaType.TABLE,
			'_bools': { 'keepEngineWarm' },
			'_strings': { 'name' },

			'Seats': {
				'_type': SchemaType.ARRAY,

				'Seat': {
					'_type': SchemaType.STRING,
				}
			},
		}

		for name, value in seat_groups.attrib.items():
			self._write(f'// Unknown attribute {name}="{value}"')

		for seat_group in seat_groups:
			if seat_group.tag != 'SeatGroup':
				self._write(f'// Unknown element "{seat_group.tag}"')
				continue

			self._begin_block()
			self._write_table(seat_group, seat_group_schema, 'seatGroup')
			self._write('')
			seat_group_name = self._lower_first_letter(seat_group.attrib['name'])
			self._write(f'VehicleSeatGroup* {seat_group_name} = new VehicleSeatGroup();')
			self._write(f'{seat_group_name}->Init(this, seatGroup);')
			self._write(f'm_seatGroups.push_back({seat_group_name});')
			self._end_block()

	################################################################################
	# Inventory
	################################################################################

	def _process_inventory(self, inventory: ET.Element):
		for child in inventory:
			if child.tag == 'AmmoTypes':
				self._process_inventory_ammo_types(child)

	def _process_inventory_ammo_types(self, ammo_types: ET.Element):
		for child in ammo_types:
			if child.tag == 'Ammo':
				self._process_inventory_ammo(child)

	def _process_inventory_ammo(self, ammo: ET.Element):
		ammo_type = ammo.attrib['type']
		ammo_capacity = ammo.attrib['capacity']

		# modifications for two variables at once are not supported
		assert 'id' not in ammo.attrib

		self._write(f'this->SetAmmoCapacity("{ammo_type}", {ammo_capacity});')

	################################################################################
	# Paints
	################################################################################

	def _process_paints(self, paints: ET.Element):
		first = True
		for paint in paints:
			if paint.tag == 'Paint':
				self._process_paints_paint(paint, first)
				first = False

	def _process_paints_paint(self, paint: ET.Element, first: bool):
		check = 'StringTools::IsEqualNoCase'
		statement = 'if' if first else 'else if'
		self._write_value(paint, 'name', f'{statement} ({check}(m_paintName, ', '))', self._convert_string)
		self._begin_block()
		self._write_value(paint, 'material', 'this->SetPaintMaterial(', ');', self._convert_string)
		self._end_block()

	################################################################################
	# Actions
	################################################################################

	def _process_actions(self, actions: ET.Element):
		actions_schema = {
			'_type': SchemaType.ARRAY,

			'Action': {
				'_type': SchemaType.TABLE,
				'_strings': { 'class' },

				'Activations': {
					'_type': SchemaType.ARRAY,

					'Activation': {
						'_type': SchemaType.TABLE,
						'_floats': { 'distance' },
						'_strings': { 'type', 'param1', 'param2' },
					}
				},

				'Enter': {
					'_type': SchemaType.TABLE,

					'Seats': {
						'_type': SchemaType.ARRAY,

						'Seat': {
							'_type': SchemaType.STRING,
						}
					},
				},

				'AutomaticDoor': {
					'_type': SchemaType.TABLE,
					'_bools': { 'disabled' },
					'_floats': { 'timeMax' },
					'_strings': { 'animation', },
				},

				'LandingGears': {
					'_type': SchemaType.TABLE,
					'_bools': { 'isOnlyAutoForPlayer' },
					'_floats': { 'landingDamages', 'altitudeToRetractGears', 'velocityMax' },
					'_strings': { 'blockPartRotation' },
				},

				'EntityAttachment': {
					'_type': SchemaType.TABLE,
					'_strings': { 'class', 'helper' },
				},
			}
		}

		self._begin_block()
		self._write_table(actions, actions_schema, 'actions')
		self._write('')
		self._write('SmartScriptTable table(gEnv->pScriptSystem);')
		self._write('table->SetValue("Actions", actions);')
		self._write('this->InitActions(table);')
		self._end_block()

	################################################################################
	# Damages
	################################################################################

	def _process_damages(self, damages: ET.Element):
		damages_schema = {
			'_type': SchemaType.TABLE,
			'_floats': {
				'submergedRatioMax', 'submergedDamageMult', 'collDamageThreshold',
				'groundCollisionMinMult', 'groundCollisionMaxMult', 'groundCollisionMinSpeed',
				'groundCollisionMaxSpeed', 'vehicleCollisionDestructionSpeed',
			},

			'DamageMultipliers': DAMAGE_MULTIPLIERS_SCHEMA,

			'DamagesGroups': {
				'_type': SchemaType.ARRAY,

				'DamagesGroup': {
					'_type': SchemaType.TABLE,
					'_strings': { 'name', 'useTemplate' },

					'DamagesSubGroups': {
						'_type': SchemaType.ARRAY,

						'DamagesSubGroup': {
							'_type': SchemaType.TABLE,
							'_floats': { 'delay', 'randomness' },

							'DamageBehaviors': DAMAGE_BEHAVIORS_SCHEMA,
						}
					},
				}
			},
		}

		self._begin_block()
		self._write_table(damages, damages_schema, 'damages')
		self._write('')
		self._write('SmartScriptTable table(gEnv->pScriptSystem);')
		self._write('table->SetValue("Damages", damages);')
		self._write('this->InitDamages(table);')
		self._end_block()

	################################################################################
	# DamagesGroup
	################################################################################

	def _process_damages_group(self, group: ET.Element):
		self._begin_block()
		self._write_table(group, DAMAGE_GROUP_SCHEMA, 'group')
		self._write('')
		name = group.attrib['name']
		self._write(f'm_groups["{name}"] = group;')
		self._end_block()

################################################################################
# Main
################################################################################

def dir_path(value: str) -> Path:
	path = Path(value)
	if not path.is_dir():
		raise argparse.ArgumentTypeError(f'"{value}" is not a directory path!')
	return path

def parse_command_line() -> argparse.Namespace:
	parser = argparse.ArgumentParser(description='Crysis vehicle XMLs to C++ code convertor')
	parser.add_argument('path', type=dir_path, help='path to extracted GameData.pak and ZPatch1.pak')
	return parser.parse_args()

def generate_vehicle_implementations(input_path: Path, output_path: Path) -> None:
	for xml_path in Path(input_path, 'Scripts/Entities/Vehicles/Implementations/Xml').glob('**/*.xml'):
		xml_root = ET.parse(xml_path).getroot()
		xml_name = xml_path.relative_to(input_path).as_posix()
		VehicleConverter(xml_root, xml_name, output_path).convert_vehicle_implementations()

def generate_damages_templates(input_path: Path, output_path: Path) -> None:
	xml_name = 'Scripts/Entities/Vehicles/DamagesTemplates/DefaultVehicleDamages.xml'
	xml_path = Path(input_path, xml_name)
	xml_root = ET.parse(xml_path).getroot()
	VehicleConverter(xml_root, xml_name, output_path).convert_damages_templates()

def main() -> int:
	args = parse_command_line()
	input_path = args.path
	output_path = Path(__file__).parent / '..' / 'Code/CryAction/Vehicles/Implementations'
	generate_vehicle_implementations(input_path, output_path)
	generate_damages_templates(input_path, output_path)
	return 0

if __name__ == '__main__':
	sys.exit(main())
