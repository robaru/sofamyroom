# from pyroomsim import py_mul_struct
# print(py_mul_struct(1, 4.2))


# from pyroomsim import make_sources
#
# hey = dict(first=1, second=2.0)
# hey2 = dict(first=12, second=22.0)
#
# s = make_sources(hey, hey2)
# print(s[0])

# WORKS
# from roomsim import make_options
# from api import Options
# options = Options()
# # For options, we'll need the get config, starts to be too much.
# hey = make_options(options.get_config())
# print(hey)

# from roomsim import make_surface
# from api import Surface
#
# surf = Surface()
# make_surface(surf.get_config())
# print(surf.get_config())


# WORKS
# from roomsim import make_room
# from api import Room
#
# room = Room()
# make_room(room.get_config())

from roomsim import make_roomsetup
from api import RoomSetup, Sources, Receivers

sources = [
    Sources(location=(8, 2.5, 1.6), orientation=(180, 0, 0), description="subcardiod"),
    Sources(location=(2, 3.3, 2.6), orientation=(0, 180, 0), description="subcardiod"),
]
# sources = Sources(location=(8, 2.5, 1.6), orientation=(180, 0, 0), description="subcardiod"),
receivers = Receivers(location=(5, 3, 0), orientation=(0, 0, 0), description="subcardioid")
setup = RoomSetup(sources=sources, receivers=receivers)
make_roomsetup(setup.get_config())


# # Worked
# from api import RoomSetup, Sources, Receivers
# from roomsim import make_sensors
# # sources = Sources(location=(8, 2.5, 1.6), orientation=(180, 0, 0), description="subcardiod")
# sources = [
#     Sources(location=(8, 2.5, 1.6), orientation=(180, 0, 0), description="subcardiod"),
#     Sources(location=(2, 3.3, 2.6), orientation=(0, 180, 0), description="subcardiod"),
# ]
#
# make_sensors([sources[0].get_config(), sources[1].get_config()])

