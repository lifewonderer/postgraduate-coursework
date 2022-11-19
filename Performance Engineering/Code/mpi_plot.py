import numpy as np
import matplotlib.pyplot as plt

data_refs = np.loadtxt('collect_mpibp1refs.out')
data_refb = np.loadtxt('collect_mpibp1refb.out')
data_avxs = np.loadtxt('collect_mpibp1avxs.out')
data_avxb = np.loadtxt('collect_mpibp1avxb.out')

for i in range(3):

    index = i*8

    n = [1000, 10000, 100000]
    p = data_refs[index:index+8,0]

    time_refs = data_refs[index:index+8,1]
    time_refb = data_refb[index:index+8,1]
    time_avxs = data_avxs[index:index+8,1]
    time_avxb = data_avxb[index:index+8,1]

    dofs_refs = data_refs[index:index+8,2]
    dofs_refb = data_refb[index:index+8,2]
    dofs_avxs = data_avxs[index:index+8,2]
    dofs_avxb = data_avxb[index:index+8,2]

    plt.figure(figsize=(6, 6))
    plt.plot(p,dofs_refs,'o-', color='black',label='S-Ref')
    plt.plot(p,dofs_refb,'o--', color='brown',label='B-Ref')
    plt.plot(p,dofs_avxs,'s-', color='darkblue',label='S-AVX')
    plt.plot(p,dofs_avxb,'s--', color='darkorange',label='B-AVX')
    plt.title('BP1 Problem size = %i' %n[i], fontsize=20)
    plt.xlabel('Number of process', fontsize=18)
    plt.ylabel('DoFs/sec in CG (million)', fontsize=18)
    plt.ticklabel_format(style='sci', scilimits=(-1,2), axis='y') # Scientific notation
    plt.legend(fontsize=14)
    plt.grid()
    plt.xscale('log', basex=2)
    plt.savefig('mpibp1_dofs_size%i.png' %n[i])
    plt.close()

data_refs = np.loadtxt('collect_mpibp2refs.out')
data_refb = np.loadtxt('collect_mpibp2refb.out')
data_avxs = np.loadtxt('collect_mpibp2avxs.out')
data_avxb = np.loadtxt('collect_mpibp2avxb.out')

for i in range(3):

    index = i*8

    n = [1000, 10000, 100000]
    p = data_refs[index:index+8,0]

    time_refs = data_refs[index:index+8,1]
    time_refb = data_refb[index:index+8,1]
    time_avxs = data_avxs[index:index+8,1]
    time_avxb = data_avxb[index:index+8,1]

    dofs_refs = data_refs[index:index+8,2]
    dofs_refb = data_refb[index:index+8,2]
    dofs_avxs = data_avxs[index:index+8,2]
    dofs_avxb = data_avxb[index:index+8,2]

    plt.figure(figsize=(6, 6))
    plt.plot(p,dofs_refs,'o-', color='black',label='S-Ref')
    plt.plot(p,dofs_refb,'o--', color='brown',label='B-Ref')
    plt.plot(p,dofs_avxs,'s-', color='darkblue',label='S-AVX')
    plt.plot(p,dofs_avxb,'s--', color='darkorange',label='B-AVX')
    plt.title('BP2 Problem size = %i' %n[i], fontsize=20)
    plt.xlabel('Number of process', fontsize=18)
    plt.ylabel('DoFs/sec in CG (million)', fontsize=18)
    plt.ticklabel_format(style='sci', scilimits=(-1,2), axis='y') # Scientific notation
    plt.legend(fontsize=14)
    plt.grid()
    plt.xscale('log', basex=2)
    plt.savefig('mpibp2_dofs_size%i.png' %n[i])
    plt.close()
