import math

def is_prime(n):
    m = int(math.ceil(math.sqrt(n)))
    for i in range(2,m+1):
        if n % i == 0 and n != i:
            return False
    return True