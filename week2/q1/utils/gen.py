from random import choice, randint
from yaml import load, Loader

def get_fake_last(fake_names):
    return choice(fake_names['last'])

def get_fake_first(fake_names):
    return choice(fake_names['first'])

def get_age():
    return randint(0, 100)

def get_month():
    return randint(1, 12)

def get_date():
    return randint(1, 28)

if __name__ == "__main__":
    with open("data/fake_name.yaml", "rb") as f:
        fake_names = load(f, Loader=Loader)

    with open("data/person_big.txt", "w+") as f:
        for i in range(100000):
            f.write(
                "%s %s %d %d %d\n" % 
                (get_fake_last(fake_names), 
                get_fake_first(fake_names), 
                get_age(), 
                get_month(), 
                get_date())
            )

