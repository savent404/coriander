from matplotlib import pyplot as plt
import numpy as np
import math

def svpwm(alpha: float, beta: float):
    sqrt3_2 = math.sqrt(3) / 2

    i = alpha * sqrt3_2
    j = -alpha /2 + beta * sqrt3_2
    k = -alpha /2 - beta * sqrt3_2

    f1 = 0
    f2 = 0
    f3 = 0
    if i > 0:
        f1 = 1
    if j > 0:
        f2 = 1
    if k > 0:
        f3 = 1

    N = f1 + f2*2 + f3*4
    n2sector = [6, 2, 1, 4, 5, 3]
    sector = n2sector[N-1]

    t1 = 0
    t2 = 0
    if sector == 1:
        t1 = i
        t2 = j
    elif sector == 2:
        t1 = -k
        t2 = -i
    elif sector == 3:
        t1 = j
        t2 = k
    elif sector == 4:
        t1 = -i
        t2 = -j
    elif sector == 5:
        t1 = k
        t2 = i
    elif sector == 6:
        t1 = -j
        t2 = -k
    else:
        t1 = 0
        t2 = 0
    
    t0 = 0.5 * (1 - t1 - t2)

    if sector == 1:
        i = t1 + t2 + t0
        j = t2 + t0
        k = t0
    elif sector == 2:
        i = t1 + t0
        j = t1 + t2 + t0
        k = t0
    elif sector == 3:
        i = t0
        j = t1 + t2 + t0
        k = t2 + t0
    elif sector == 4:
        i = t0
        j = t1 + t0
        k = t1 + t2 + t0
    elif sector == 5:
        i = t2 + t0
        j = t0
        k = t1 + t2 + t0
    elif sector == 6:
        i = t1 + t2 + t0
        j = t0
        k = t1 + t0
    else:
        i = 0
        j = 0
        k = 0
        
    return i,j,k

def emulator_key_point():

    for idx in range(36):
        angle = idx * math.pi / 18
        alpha = math.cos(angle)
        beta = math.sin(angle)
        i,j,k = svpwm(alpha, beta)
        print("idx: %02d, angle: %03d ,alpha: %.2f, beta: %.2f, i: %.10f, j: %.10f, k: %.10f" % (idx, idx*10, alpha, beta, i, j, k))

if __name__ == '__main__':

    emulator_key_point()

    # show the wave to verify the correctness
    t = np.linspace(0, 0.01, 1000)
    f = 500
    w = 2 * math.pi * f

    alpha = 1 * np.sin(w * t)
    beta = 1 * np.cos(w * t)

    u_a = []
    u_b = []
    u_c = []
    
    for i in range(len(t)):
        a,b,c = svpwm(alpha[i], beta[i])
        u_a.append(a)
        u_b.append(b)
        u_c.append(c)
    
    
    plt.plot(t, alpha, label='alpha')
    plt.plot(t, beta, label='beta')
    plt.plot(t, u_a, label='u_a')
    plt.plot(t, u_b, label='u_b')
    plt.plot(t, u_c, label='u_c')
    plt.legend()
    plt.show()
    