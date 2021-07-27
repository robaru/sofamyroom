import functools
import inspect
import re
from pprint import pprint


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


def dict_from_lines(line_list):
    all_lines = list(line_list)  # Copy
    conf = dict()
    # We will pop, so reverse to start from the top.
    all_lines.reverse()
    print("")
    while all_lines:
        line = all_lines.pop()
        # Pass commented and empty lines
        if line.strip().startswith('%') or not line.strip():
            continue
        # Concatenate list values over several lines.
        if "[" in line and "]" not in line:
            tmp = ""
            while "]" not in line:
                tmp += line.strip().replace(";", " ")  # Remove the last ";"
                line = all_lines.pop()
            tmp += line.strip()
            line = tmp
        # Build conf
        tmp = line.split('=')
        field, value = tmp[0].strip(), tmp[1].strip()
        # Convert .txt value to Python value.
        value = convert_value(value.split(";")[0])

        # Fill in recursively. (Keep source(1) keys)
        field_tmp = field.split(".")
        f_d = conf
        for f in field_tmp[:-1]:
            if f not in f_d:
                f_d[f] = dict()
            f_d = f_d[f]
        f_d[field_tmp[-1]] = value

    # Merge sources and receivers as list of dict.
    # Gets 13 in the string "source(13)" etc.. to sort the list.
    sort_func = lambda x: int(re.findall(r"\d+", x)[0])
    # Merge source(1) and source(2) etc.. together.
    source_k = [k for k in conf.keys() if k.startswith("source")]
    source_k.sort(key=sort_func)
    dict_list = []
    for k in source_k:
        dict_list.append(conf.pop(k))
    conf["sources"] = dict_list

    receiver_k = [k for k in conf.keys() if k.startswith("receiver")]
    receiver_k.sort(key=sort_func)
    dict_list = []
    for k in receiver_k:
        dict_list.append(conf.pop(k))
    conf["receivers"] = dict_list
    return conf


def isint(v):
    try:
        v = int(v)
        return True
    except ValueError:
        return False


def isfloat(v):
    try:
        v = float(v)
        return True
    except ValueError:
        return False


def islist(v):
    return "[" in v and "]" in v


def to_list(v):
    # Handle list with several spaces.
    return [convert_value(u) for u in v.split(" ")[1:-1] if u]


def convert_value(v: str):
    # Bool
    if v == "true":
        return True
    if v == "false":
        return False
    # Int
    if isint(v):
        return int(v)
    # Float
    if isfloat(v):
        return float(v)
    # List
    if islist(v):
        return to_list(v)
    return v
