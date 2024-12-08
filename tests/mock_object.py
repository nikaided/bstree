class NoCmpObj:
    def __init__(self, val):
        self.val = val

    def __sub__(self, other):
        return self.val - other.val

class LTObj:
    def __init__(self, val):
        self.val = val

    def __lt__(self, other):
        return self.val < other.val
    
    def __sub__(self, other):
        return self.val - other.val

class GTObj:
    def __init__(self, val):
        self.val = val

    def __gt__(self, other):
        return self.val > other.val
    
    def __sub__(self, other):
        return self.val - other.val

class LTGTObj:
    def __init__(self, val):
        self.val = val

    def __lt__(self, other):
        return self.val < other.val

    def __gt__(self, other):
        return self.val > other.val
    
    def __sub__(self, other):
        return self.val - other.val
    
class HashableObj:
    def __init__(self, val):
        self.val = val

    def __hash__(self):
        return hash(self.val)

    def __eq__(self, other):
        return self.val == other.val
