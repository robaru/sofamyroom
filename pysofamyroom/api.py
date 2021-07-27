import yaml
from typing import Tuple, Union, List
from scipy.io.wavfile import write

# from pysofamyroom import _roomsim, release_brir, clear_sensors
from pysofamyroom.utils import get_class_args, dict_from_lines


class BaseClass:
    def get_config(self, with_name=False):
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
                # Handle list of classes.
                return [v if not has_gc(v) else v.get_config() for v in val]
            return post_process(val)
        # self._conf_keys is defined in get_class_args
        conf = {k: recursive_getattr(self, k) for k in self._conf_keys}
        # Might be useful to have `from_config`.
        if with_name:
            conf["name"] = self.__class__.__name__
        return conf

    def to_yaml(self, filename: str, sort_keys=False):
        def rec_trav(d):
            if isinstance(d, bytes):
                return d.decode("utf-8")
            if isinstance(d, dict):
                return {k: rec_trav(v) for k, v in d.items()}
            if isinstance(d, list):
                return [rec_trav(v) for v in d]
            return d

        dic = self.get_config()
        dic = rec_trav(dic)
        if not filename.endswith(".yml"):
            filename += ".yml"
        yaml.safe_dump(dic, open(filename, "w"), default_flow_style=None, sort_keys=sort_keys)

    @classmethod
    def from_config(cls, conf):
        def get_class(k) -> BaseClass:
            return {k.lower(): v for k, v in globals().items()}.get(k, None)
        # Top conf is a dict of dict.
        kwargs = {}
        for k, d in conf.items():
            if isinstance(d, dict):
                # Instantiate the subclasses (surface, room etc..)
                kwargs[k] = get_class(k).from_config(d)
            elif isinstance(d, list):
                sub_cls = get_class(k)
                if sub_cls is None:
                    # Normal field, keep the list
                    kwargs[k] = d
                else:
                    # Instantiate list of classes.
                    kwargs[k] = [sub_cls.from_config(sub_d) for sub_d in d]
            else:
                kwargs[k] = d
        return cls(**kwargs)

    @classmethod
    def from_yaml(cls, filename):
        dic = yaml.safe_load(open(filename, "r"))
        return cls.from_config(dic)

    @classmethod
    def from_txt(cls, filename):
        with open(filename, "r") as f:
            all_lines = f.readlines()
        conf = dict_from_lines(all_lines)
        return cls.from_config(conf)
    @classmethod
    def write_wavefile(sample_rate,data,id):
        write(id+".wav",sample_rate,data)

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
                0.1000, 0.0500, 0.0600, 0.0700, 0.1000, 0.1000,
                0.1400, 0.3500, 0.5300, 0.7500, 0.7000, 0.6000,
                0.1000, 0.0500, 0.0600, 0.7000, 0.1000, 0.1000,
                0.1000, 0.0500, 0.0600, 0.0700, 0.1000, 0.1000,
                0.0100, 0.0200, 0.0600, 0.1500, 0.2500, 0.4500,
                0.2400, 0.1900, 0.1400, 0.0800, 0.1300, 0.1000
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
            simulatediffuse: bool = True,
            numberofrays: int = 2000,
            diffusetimestep: float = 0.010,
            rayenergyfloordB: float = -80,
            uncorrelatednoise: bool = True,
            **kwargs
    ):
        pass


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

    def generate(self):
        from pysofamyroom.internal import generate
        wav, channels, fs, n_samples = generate(self.get_config())
        return wav
    #

# def roomsim(room_setup: RoomSetup, other_options):
#     samples = _roomsim(room_setup)  # Include validate samples
#     # Is that necessary? IDK
#     release_brir()
#     clear_sensors()
#     return samples
