class LINQ:
    def __init__(self, gen):
        self.gen = gen

    def Select(self, func):
        gen_copy = self.gen
        def new_gen_select():
            for i in  gen_copy:
                yield func(i)
        self.gen = new_gen_select()
        return self

    def Flatten(self):
        new = []
        gen_copy = self.gen
        for i in gen_copy:
            for j in i:
                new.append(j)

        def new_gen_flatten():
            for i in new:
                yield i

        self.gen = new_gen_flatten()
        return self

    def Where(self, func):
        gen_copy = self.gen

        def new_gen_where():
            for i in gen_copy:
                if func(i):
                    yield i

        self.gen = new_gen_where()
        return self

    def Take(self, k):
        gen_copy = self.gen

        def new_gen_take():
            count = 0
            for i in gen_copy:
                if (count < k):
                    count += 1
                    yield i
                else:
                    break

        self.gen = new_gen_take()
        return self


    def GroupBy(self, func):
        gen_copy = self.gen
        new = {}
        for i in gen_copy:
            new_i = func(i)
            if new_i in new:
                new[new_i].append(i)
            else:
                new[new_i] = [i]

        def new_gen_groupby():
            for i in new:
                yield [i, new[i]]

        self.gen = new_gen_groupby()
        return self

    def OrderBy(self, func):
        gen_copy = self.gen
        new = sorted(gen_copy, key=func)

        def new_gen_orderby():
            for i in new:
                yield i

        self.gen = new_gen_orderby()
        return self


    def ToList(self):
        gen_copy = self.gen
        new = []
        for i in gen_copy:
            new.append(i)
        return new

'''
def my_gen(a):
    for i in a:
        yield i

l = [9, 1, 3, 2, 6, 7, 4, 8, 5]
a = LINQ(my_gen(l))
print(a.OrderBy(lambda x: x).ToList())
'''

def Fibonachi():
    first = 1
    second = 1
    while True:
        yield first
        temp = first
        first = second
        second = second + temp

def getLine(f):
    for string in f:
        yield string


a = LINQ(Fibonachi())
print (a.Where(lambda x: True if x % 3 == 0 else False).Select(lambda x: x^2 if x % 2 == 0 else x).Take(5).ToList())

f = open('article.txt', 'r')
b = LINQ(getLine(f))
print(b.Select(lambda x: x.split()).Flatten().GroupBy(lambda x: x).Select(lambda x: [x[0], len(x[1])]).OrderBy(lambda x: x[1]).ToList())