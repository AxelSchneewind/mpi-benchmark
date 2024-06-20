
import random


class intervals:
    left = list()
    right = list()

    count = 0

    def __init__(self, count):
        self.count = count
        self.left  = [-1] * count
        self.right = [-1] * count

    def insert(self, number):
        self.left[number] = number
        self.right[number] = number + 1

        # get left bound from previous entry
        if number > 0 and self.left[number - 1] > -1:
            self.left[number] = self.left[number - 1]

        # get right bound from next entry
        if number < self.count - 1 and self.right[number + 1] > -1:
            self.right[number] = self.right[number + 1]

        # update previous entries
        if self.left[number] > 0:
            self.right[self.left[number]] = self.right[number]

        # update next entries
        self.left[self.right[number] - 1] = self.left[number]

    def pull_largest_interval(self):
        index = max(range(self.count), key=lambda j: self.right[j] - self.left[j])

        # 
        result = (self.left[index], self.right[index])

        # reset counters
        for j in range(self.left[index], self.right[index]):
            self.left[j] = -1
            self.right[j] = -1

        return result

    def pull_first_interval(self, size):
        for i in range(self.count):
            if self.right[i] - self.left[i] >= size:
                result = (self.left[i], self.right[i])

                # reset counters
                for j in range(self.left[i], self.right[i]):
                    self.left[j] = -1
                    self.right[j] = -1

                return result

        return None

    def __str__(self):
        result = ''
        for i in range(self.count):
            result = result + str((self.left[i], self.right[i])) + ', '

        return result


def test():
    # numbers to insert
    N = 10000
    numbers = list(range(N))
    random.shuffle(numbers)
    print('inserting  ', numbers)

    
    aggregator = intervals(N)
    
    # fill data structure without pulling numbers
    for i in numbers:
        aggregator.insert(i)

    print('datastructure: ', aggregator)

        
    # # fill data structure and pull intervals as soon as they hit threshold
    # threshold = 8
    # aggregator = intervals(N)
    # pulled_numbers = []
    # for i in numbers:
    #     # print('inserting ', i)
    #     aggregator.insert(i)

    #     interval = aggregator.pull_first_interval(threshold)

    #     if interval is not None:
    #         # print('found interval: [', interval[0], interval[1], ']')
    #         pulled_numbers.extend(range(interval[0], interval[1]))

    # while threshold > 0:
    #     interval = aggregator.pull_first_interval(threshold)
    #     
    #     if interval is None:
    #         threshold = threshold / 2
    #         continue
    #     else:
    #         print('found interval: [', interval[0], interval[1], ']')
    #         pulled_numbers.extend(range(interval[0], interval[1]))

    # print('extracted ', len(pulled_numbers), ' entries')
    # print('extraction correct: ', sorted(pulled_numbers) == sorted(numbers))

    # fill data structure and pull intervals starting with the largest
    aggregator = intervals(N)
    pulled_numbers = []

    for i in numbers:
        aggregator.insert(i)

    # pull largest interval
    interval = aggregator.pull_largest_interval()
    print('found interval: [', interval[0], interval[1], ']')
    pulled_numbers.extend(range(interval[0], interval[1]))

    while True:
        interval = aggregator.pull_largest_interval()
        if interval[1] - interval[0] > 0:
            print('found interval: [', interval[0], interval[1], ']')
            pulled_numbers.extend(range(interval[0], interval[1]))
        else:
            break

    print('extracted ', len(pulled_numbers), ' entries')
    print('extraction correct: ', sorted(pulled_numbers) == sorted(numbers))





if __name__ == '__main__':
    test()
