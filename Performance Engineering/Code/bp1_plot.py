import numpy as np
import matplotlib.pyplot as plt

data_refs = np.loadtxt('collect_bp1refs.out')
data_refb = np.loadtxt('collect_bp1refb.out')
data_avxs = np.loadtxt('collect_bp1avxs.out')
data_avxb = np.loadtxt('collect_bp1avxb.out')

for i in range(12):

    index = i*16

    n = data_refs[index:index+16,0]
    p = data_refs[index:index+16,1]

    time_refs = data_refs[index:index+16,2]
    time_refb = data_refb[index:index+16,2]
    time_avxs = data_avxs[index:index+16,2]
    time_avxb = data_avxb[index:index+16,2]

    dofs_refs = data_refs[index:index+16,3]
    dofs_refb = data_refb[index:index+16,3]
    dofs_avxs = data_avxs[index:index+16,3]
    dofs_avxb = data_avxb[index:index+16,3]
    
    '''
    plt.figure(figsize=(6, 6))
    plt.plot(p,time_refs,'o-', color='black',label='S-Ref')
    plt.plot(p,time_refb,'o--', color='brown',label='B-Ref')
    plt.plot(p,time_avxs,'s-', color='darkblue',label='S-AVX')
    plt.plot(p,time_avxb,'s--', color='darkorange',label='B-AVX')
    plt.title('BP1 Problem size = %i' %n[0], fontsize=20)
    plt.xlabel('Polynomial degree', fontsize=18)
    plt.ylabel('CG Solve time (sec)', fontsize=18)
    plt.ticklabel_format(style='sci', scilimits=(-1,2), axis='y') # Scientific notation
    plt.legend(fontsize=14)
    plt.grid()
    plt.xticks(np.arange(1, 17))
    plt.savefig('bp1_time_size%i.png' %n[0])
    plt.close()
    '''

    plt.figure(figsize=(6, 6))
    plt.plot(p,dofs_refs,'o-', color='black',label='S-Ref')
    plt.plot(p,dofs_refb,'o--', color='brown',label='B-Ref')
    plt.plot(p,dofs_avxs,'s-', color='darkblue',label='S-AVX')
    plt.plot(p,dofs_avxb,'s--', color='darkorange',label='B-AVX')
    plt.title('BP1 Problem size = %i' %n[0], fontsize=20)
    plt.xlabel('Polynomial degree', fontsize=18)
    plt.ylabel('DoFs/sec in CG (million)', fontsize=18)
    plt.ticklabel_format(style='sci', scilimits=(-1,2), axis='y') # Scientific notation
    plt.legend(fontsize=14)
    plt.grid()
    plt.xticks(np.arange(1, 17))
    plt.savefig('bp1_dofs_size%i.png' %n[0])
    plt.close()

for i in range(16):

    index = np.arange(i,192,step=16)

    n = data_refs[index,0]
    p = data_refs[index,1]

    time_refs = data_refs[index,2]
    time_refb = data_refb[index,2]
    time_avxs = data_avxs[index,2]
    time_avxb = data_avxb[index,2]

    dofs_refs = data_refs[index,3]
    dofs_refb = data_refb[index,3]
    dofs_avxs = data_avxs[index,3]
    dofs_avxb = data_avxb[index,3]
    
    '''
    plt.figure(figsize=(6, 6))
    plt.plot(n,time_refs,'o-', color='black',label='S-Ref')
    plt.plot(n,time_refb,'o--', color='brown',label='B-Ref')
    plt.plot(n,time_avxs,'s-', color='darkblue',label='S-AVX')
    plt.plot(n,time_avxb,'s--', color='darkorange',label='B-AVX')
    plt.title('BP1 Polynomial degree = %i' %p[0], fontsize=20)
    plt.xlabel('Problem size', fontsize=18)
    plt.ylabel('CG Solve time (sec)', fontsize=18)
    plt.xscale('log')
    plt.ticklabel_format(style='sci', scilimits=(-1,2), axis='y') # Scientific notation
    plt.legend(fontsize=14)
    plt.grid()
    plt.savefig('bp1_time_degree%i.png' %p[0])
    plt.close()
    '''

    plt.figure(figsize=(6, 6))
    plt.plot(n,dofs_refs,'o-', color='black',label='S-Ref')
    plt.plot(n,dofs_refb,'o--', color='brown',label='B-Ref')
    plt.plot(n,dofs_avxs,'s-', color='darkblue',label='S-AVX')
    plt.plot(n,dofs_avxb,'s--', color='darkorange',label='B-AVX')
    plt.title('BP1 Polynomial degree = %i' %p[0], fontsize=20)
    plt.xlabel('Problem size', fontsize=18)
    plt.ylabel('DoFs/sec in CG (million)', fontsize=18)
    plt.xscale('log')
    plt.ticklabel_format(style='sci', scilimits=(-1,2), axis='y') # Scientific notation
    plt.legend(fontsize=14)
    plt.grid()
    plt.savefig('bp1_dofs_degree%i.png' %p[0])
    plt.close()