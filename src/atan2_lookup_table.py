from numpy import arctan2, pi, ndarray

results = ndarray((128, 128))

for x in range(0, 128):
    for y in range(0, 128):
        results[x][y] = round(180.0/pi * arctan2(y/128.0, x/128.0))

print(results)



    




