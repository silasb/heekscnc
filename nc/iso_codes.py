SPACE = ''
FORMAT_IN = '%.5f'
FORMAT_MM = '%.3f'
FORMAT_ANG = '%.1f'
FORMAT_TIME = '%.2f'

BLOCK = 'N%i' + SPACE
COMMENT = '(%s)'
VARIABLE = '#%i'
VARIABLE_SET = '=%.3f'

PROGRAM = 'O%i'
PROGRAM_END = 'M02'

SUBPROG_CALL = 'M98' + SPACE + 'P%i'
SUBPROG_END = 'M99'

STOP_OPTIONAL = 'M01'
STOP = 'M00'

IMPERIAL = SPACE + 'G20'
METRIC = SPACE + 'G21'
ABSOLUTE = SPACE + 'G90'
INCREMENTAL = SPACE + 'G91'
POLAR_ON = SPACE + 'G16'
POLAR_OFF = SPACE + 'G15'
PLANE_XY = SPACE + 'G17'
PLANE_XZ = SPACE + 'G18'
PLANE_YZ = SPACE + 'G19'

TOOL = 'T%i' + SPACE + 'M06'

WORKPLANE = 'G%i'
WORKPLANE_BASE = 53

FEEDRATE = SPACE + 'F'
SPINDLE = SPACE + 'S' + FORMAT_ANG
COOLANT_OFF = SPACE + 'M09'
COOLANT_MIST = SPACE + 'M07'
COOLANT_FLOOD = SPACE + 'M08'
GEAR_OFF = SPACE + '?'
GEAR = 'M%i'
GEAR_BASE = 37

RAPID = 'G00'
FEED = 'G01'
ARC_CW = 'G02'
ARC_CCW = 'G03'
DWELL = 'G04'

X = SPACE + 'X'
Y = SPACE + 'Y'
Z = SPACE + 'Z'
A = SPACE + 'A'
B = SPACE + 'B'
C = SPACE + 'C'
CENTRE_X = SPACE + 'I'
CENTRE_Y = SPACE + 'J'
CENTRE_Z = SPACE + 'K'
RADIUS = SPACE + 'R'
TIME = SPACE + 'P'
