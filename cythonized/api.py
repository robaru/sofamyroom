from typing import Tuple

# from pysofamyroom import _roomsim, release_brir, clear_sensors
from utils import get_class_args


class BaseClass:
    def get_config(self):
        # Does this work?
        def recursive_getattr(cls, k):
            val = getattr(cls, k)
            if getattr(val, "get_config", None) is not None:
                return val.get_config()
            return val

        # self._conf_keys is defined in get_class_args
        return {k: recursive_getattr(self, k) for k in self._conf_keys}
    # TODO: from config? Will need self.__class__.__name__ then, why not?


class Surface(BaseClass):
    # There are arrays here, find out what to do.
    # Check memory views, that might be the way
    # https://docs.cython.org/en/latest/src/userguide/memoryviews.html
    @get_class_args
    def __init__(self, hey=1, bis=2):
        pass


class Room(BaseClass):
    @get_class_args
    def __init__(
            self,
            dimension: Tuple = (10, 7, 4),
            humidity: float = 0.42,
            temperature: float = 20.,
            surface: Surface = None,
    ):
        pass


class Options(BaseClass):
    @get_class_args
    def __init__(
            self,
            fs: float = 44100,
            responseduration: float = 1.25,
            bandsperoctave: float = 1,
            referencefrequency: float = 125,
            airabsorption: bool = True,
            distanceattenuation: bool = True,
            subsampleaccuracy: bool = False,
            highpasscutoff: float = 0,
            verbose: bool = True,
            simulatespecular: bool = True,
            reflectionorder: Tuple = (10, 10, 10),
            simulatediffuse: bool = False,
            numberofrays: int = 2000,
            diffusetimestep: float = 0.010,
            rayenergyfloordB: float = -80.,
            uncorrelatednoise: bool = True,
            # outputname: str = None,
    ):

        pass

# FIXME: Need to enable list of Source.
# Is the Sources, or Room handling the list?
class Sources(BaseClass):
    @get_class_args
    def __init__(
            self,
            location: Tuple = None,
            orientation: Tuple = None,
            description: str = None,
    ):
        pass


Receivers = Sources


class RoomSetup:
    @get_class_args
    def __init__(self, room: Room, options: Options, sources: Sources, receivers: Receivers):
        pass

    @classmethod
    def from_yaml(cls, file):
        pass

    @classmethod
    def from_txt(cls, file):
        pass

    @classmethod
    def from_args(cls, *, all, the, args, of, other, classes):
        pass

    # def generate(self, **kwargs):
    #     return roomsim(self, **kwargs)
    #

# def roomsim(room_setup: RoomSetup, other_options):
#     samples = _roomsim(room_setup)  # Include validate samples
#     # Is that necessary? IDK
#     release_brir()
#     clear_sensors()
#     return samples
