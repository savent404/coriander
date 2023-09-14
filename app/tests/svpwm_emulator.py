from matplotlib import pyplot as plt
import numpy as np
import math

def svpwm_raw(alpha: float, beta: float, switch_duration:float, Udc: float):
    theta = math.atan2(beta, alpha) - math.pi
    if theta < 0:
        theta = theta + math.pi * 2
    sector = int(math.floor(theta / (math.pi / 3)))
    angle = theta - sector * math.pi / 3
    u_ref = min(math.sqrt(alpha * alpha + beta * beta), Udc) * math.sqrt(3)
    u_ref_percent = u_ref / Udc
    t_halfsample = switch_duration / 2
    t1 = u_ref_percent * math.sin(math.pi/3 - angle) * t_halfsample
    t2 = u_ref_percent * math.sin(angle) * t_halfsample
    t0_half = (t_halfsample - t1 - t2) / 2

    f0 = t0_half
    f1 = t0_half + t1
    f2 = t0_half + t2
    f3 = t0_half + t1 + t2

    i,j,k = 0,0,0
    if sector == 0:
        i,j,k = f0,f1,f3
    elif sector == 1:
        i,j,k = f2,f0,f3
    elif sector == 2:
        i,j,k = f3,f0,f1
    elif sector == 3:
        i,j,k = f3,f2,f0
    elif sector == 4:
        i,j,k = f1,f3,f0
    elif sector == 5:
        i,j,k = f0,f3,f2

    # centerize
    min_v = min(i,j,k)
    max_v = max(i,j,k)
    mid_v = (max_v + min_v) / 2
    offset = t_halfsample - mid_v
    i,j,k = i + offset, j + offset, k + offset

    assert(i > 0 and j > 0 and k > 0)
    assert(i < switch_duration and j < switch_duration and k < switch_duration)

    return i,j,k

def svpwm(alpha: float, beta: float):
    return svpwm_raw(alpha, beta, 1, 1)

def emulator_key_point():
    for idx in range(36):
        angle = idx * math.pi / 18
        alpha = math.cos(angle)
        beta = math.sin(angle)
        i,j,k = svpwm(alpha, beta)
        print("idx: %02d, angle: %03d, alpha: %.2f, beta: %.2f, i: %.10f, j: %.10f, k: %.10f" % (idx, idx*10, alpha, beta, i, j, k))

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
