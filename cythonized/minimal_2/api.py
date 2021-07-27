from typing import Tuple, Union, List

# from pysofamyroom import _roomsim, release_brir, clear_sensors
from utils import get_class_args


class BaseClass:
    def get_config(self):
        # Does this work?

        def has_gc(v):
            return getattr(v, "get_config", None) is not None

        def post_process(v):
            if isinstance(v, str):
                return bytes(v.encode("UTF-8"))
            return v

        def recursive_getattr(cls, k):
            val = getattr(cls, k)
            if has_gc(val):
                return val.get_config()
            elif isinstance(val, (list, tuple)):
                # Handle list of classes. Could be prettier.
                return [v if not has_gc(v) else v.get_config() for v in val]
                # if getattr(val[0], "get_config", None) is not None:
                #     return [v.get_config() for v in val]
            return post_process(val)

        # self._conf_keys is defined in get_class_args
        return {k: recursive_getattr(self, k) for k in self._conf_keys}
    # TODO: from config? Will need self.__class__.__name__ then, why not?


class Surface(BaseClass):
    # There are arrays here, find out what to do.
    # Check memory views, that might be the way
    # https://docs.cython.org/en/latest/src/userguide/memoryviews.html
    @get_class_args
    def __init__(
            self,
            frequency=None,
            absorption=None,
            diffusion=None
    ):
        # TODO: asserts about shapes
        if self.frequency is None:
            self.frequency = [125, 250, 500, 1000, 2000, 4000]
        if self.absorption is None:
            self.absorption = [
                1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
                1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
                1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
                1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
                1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
                1.0, 1.0, 1.0, 1.0, 1.0, 1.0
            ]
        if self.diffusion is None:
            self.diffusion = [
                0.5, 0.5, 0.5, 0.5, 0.5, 0.5,
                0.5, 0.5, 0.5, 0.5, 0.5, 0.5,
                0.5, 0.5, 0.5, 0.5, 0.5, 0.5,
                0.5, 0.5, 0.5, 0.5, 0.5, 0.5,
                0.5, 0.5, 0.5, 0.5, 0.5, 0.5,
                0.5, 0.5, 0.5, 0.5, 0.5, 0.5
            ]


class Room(BaseClass):
    @get_class_args
    def __init__(
            self,
            dimension: Tuple = (10, 7, 4),
            humidity: float = 0.42,
            temperature: float = 20.,
            surface: Surface = None,
    ):
        if surface is None:
            self.surface = Surface()


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


class RoomSetup(BaseClass):
    @get_class_args
    def __init__(
            self,
            room: Room = None,
            options: Options = None,
            sources: Union[Sources, List[Sources]] = None,
            receivers: Union[Receivers, List[Receivers]] = None
    ):
        if room is None:
            self.room = Room()
        if options is None:
            self.options = Options()

        if sources is None:
            self.sources = Sources()
        # FIXME: that's a temp fix. Setting it by hand again will make trouble
        if not isinstance(self.sources, (list, tuple)):
            self.sources = [self.sources]

        if receivers is None:
            self.receivers = Receivers()
        # FIXME: that's a temp fix. Setting it by hand again will make trouble
        if not isinstance(self.receivers, (list, tuple)):
            self.receivers = [self.receivers]
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

    def generate(self):
        from roomsim import generate
        wav, channels, fs, n_samples = generate(self.get_config())
        return wav
    #

# def roomsim(room_setup: RoomSetup, other_options):
#     samples = _roomsim(room_setup)  # Include validate samples
#     # Is that necessary? IDK
#     release_brir()
#     clear_sensors()
#     return samples
