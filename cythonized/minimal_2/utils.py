import functools
import inspect


def get_class_args(cls_init):
    """ Decorator for __init__:
        - Set all arguments as attributes
        - Register _conf_keys, list of argument name.
    """

    @functools.wraps(cls_init)  # Keep signature
    def _init(self, *args, **kwargs):
        # Get all args as a dict
        all_args = all_args_as_dict(cls_init, *args, **kwargs)
        # self.k = v
        for k, v in all_args.items():
            setattr(self, k, v)
        # Register only keys so values can be changed.
        self._conf_keys = list(all_args.keys())
        # Real init is called. setattr can be overwritten.
        cls_init(self, *args, **kwargs)
    return _init


def all_args_as_dict(f, *args, **kwargs):
    """ Returns all the args that f received as a dict (pos + kwargs + default)."""
    # Inspect signature
    sig = inspect.signature(f)
    # Get kwarg defaults
    all_args = {
        k: v.default
        for k, v in sig.parameters.items()
        if v.default is not inspect.Parameter.empty
    }
    # Update with received kwargs
    all_args.update(**kwargs)
    # Get positional args (including kwargs feed as positional), as dict
    filter_keys = [k for k in sig.parameters.keys() if k != "self"]
    pos_arg_dic = {f"{k}": args[i] for i, k in enumerate(filter_keys) if i < len(args)}
    # Update the dict (overwrite default kwargs that were passed as positional)
    all_args.update(**pos_arg_dic)
    return all_args
