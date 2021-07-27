from utils import get_class_args


class TestClass:
    @get_class_args
    def __init__(self, pos, pos2, key=3, key2=6, *, hey=3):
        pass

    def get_config(self):
        return {k: getattr(self, k) for k in self._conf_keys}


if __name__ == '__main__':
    hey = TestClass(1, 2, 3)
    hey2 = TestClass(pos2=6, pos=4, key=7)
    hey3 = TestClass(1, 2, 3, 4, hey=5)
    hey4 = TestClass(hey=18, pos=1, pos2=4)

    for h in [hey, hey2, hey3, hey4]:
        print(h.get_config())
