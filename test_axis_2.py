import matplotlib 
matplotlib.use("Agg")
from pysofamyroom.internal import make_roomsetup, generate
from pysofamyroom.api import RoomSetup, Sources, Receivers
import h5py
import numpy as np
from scipy import signal
from tqdm import tqdm
import yaml
import matplotlib.pyplot as plt


path="/home/psrivastava/baseline/sofamyroom_2/"
def cal_roomsim(src_pos):
    with open(path+'conf_sim_params.yml') as file_c,open(path+'conf_room_setup_1.yml') as file_1 , open(path+'conf_receivers_1.yml') as file_2,open(path+'conf_source_1.yml') as file_4:
        option_list=yaml.load(file_c,Loader=yaml.FullLoader)
        conf_room_setup=yaml.load(file_1,Loader=yaml.FullLoader)
        rcv_file=yaml.load(file_2,Loader=yaml.FullLoader)
        #src_file=yaml.load(file_3,Loader=yaml.FullLoader)
        #src_noise_file=yaml.load(file_4,Loader=yaml.FullLoader)

    no_of_rooms=1
    no_of_view_points=1

#Output_file=h5py.File("generated_rif_test_.hdf5",'w')
#rirs_noise=Output_file.create_group('rif')

    for i in tqdm(range(no_of_rooms)):
        i=i+1
        #rir_noise_id=rirs_noise.create_group('room_'+str(i))
        #noise_rir=np.empty((2,60000))
        for j in range(no_of_view_points):
            #sources=Sources(location=src_file['room_'+str(i)]['source_pos'][j], orientation=src_file['room_'+str(i)]['source_ypr'][j], description=src_file['room_'+str(i)]['source_description'][j][0])
            noise_source=Sources(location=src_pos,orientation=[0,0,0],description="omnidirectional")
            #noise_source=Sources(location=src_noise_file['room_'+str(i)]['source_pos'][j], orientation=src_noise_file['room_'+str(i)]['source_ypr'][j], description=src_noise_file['room_'+str(i)]['source_description'][j][0])
        
            mic_pos1=Receivers(location=rcv_file['room_'+str(i)]['mic_pos_1'][j],orientation=[0,0,0], description='omnidirectional')
            mic_pos2=Receivers(location=rcv_file['room_'+str(i)]['mic_pos_2'][j],orientation=[0,0,0], description='omnidirectional')
        
            #mic_pos3=Receivers(location=rcv_file['room_'+str(i)]['mic_pos_3'][j],orientation=[0,0,0], description='omnidirectional')
            #mic_pos4=Receivers(location=rcv_file['room_'+str(i)]['mic_pos_4'][j],orientation=[0,0,0], description='omnidirectional')
            #mic_pos5=Receivers(location=rcv_file['room_'+str(i)]['mic_pos_5'][j],orientation=[0,0,0], description='omnidirectional')
            #mic_pos6=Receivers(location=rcv_file['room_'+str(i)]['mic_pos_6'][j],orientation=[0,0,0], description='omnidirectional')
            #mic_pos7=Receivers(location=rcv_file['room_'+str(i)]['mic_pos_7'][j],orientation=[0,0,0], description='omnidirectional')
            #mic_pos8=Receivers(location=rcv_file['room_'+str(i)]['mic_pos_8'][j],orientation=[0,0,0], description='omnidirectional')
            #mic_pos9=Receivers(location=rcv_file['room_'+str(i)]['mic_pos_9'][j],orientation=[0,0,0], description='omnidirectional')
            #mic_pos10=Receivers(location=rcv_file['room_'+str(i)]['mic_pos_10'][j],orientation=[0,0,0], description='omnidirectional')
            #mic_pos11=Receivers(location=rcv_file['room_'+str(i)]['mic_pos_11'][j],orientation=[0,0,0], description='omnidirectional')
            #mic_pos12=Receivers(location=rcv_file['room_'+str(i)]['mic_pos_12'][j],orientation=[0,0,0], description='omnidirectional')
        
            '''
            mic_pos13=Receivers(location=rcv_file['room_'+str(i)]['mic_pos_13'][j],orientation=[0,0,0], description='omnidirectional')
            mic_pos14=Receivers(location=rcv_file['room_'+str(i)]['mic_pos_14'][j],orientation=[0,0,0], description='omnidirectional')
            mic_pos15=Receivers(location=rcv_file['room_'+str(i)]['mic_pos_15'][j],orientation=[0,0,0], description='omnidirectional')
            mic_pos16=Receivers(location=rcv_file['room_'+str(i)]['mic_pos_16'][j],orientation=[0,0,0], description='omnidirectional')
            mic_pos17=Receivers(location=rcv_file['room_'+str(i)]['mic_pos_17'][j],orientation=[0,0,0], description='omnidirectional')
            mic_pos18=Receivers(location=rcv_file['room_'+str(i)]['mic_pos_18'][j],orientation=[0,0,0], description='omnidirectional')
            mic_pos19=Receivers(location=rcv_file['room_'+str(i)]['mic_pos_19'][j],orientation=[0,0,0], description='omnidirectional')
            mic_pos20=Receivers(location=rcv_file['room_'+str(i)]['mic_pos_20'][j],orientation=[0,0,0], description='omnidirectional')
            mic_pos21=Receivers(location=rcv_file['room_'+str(i)]['mic_pos_21'][j],orientation=[0,0,0], description='omnidirectional')
            mic_pos22=Receivers(location=rcv_file['room_'+str(i)]['mic_pos_22'][j],orientation=[0,0,0], description='omnidirectional')
            mic_pos23=Receivers(location=rcv_file['room_'+str(i)]['mic_pos_23'][j],orientation=[0,0,0], description='omnidirectional')
            mic_pos24=Receivers(location=rcv_file['room_'+str(i)]['mic_pos_24'][j],orientation=[0,0,0], description='omnidirectional')
            mic_pos25=Receivers(location=rcv_file['room_'+str(i)]['mic_pos_25'][j],orientation=[0,0,0], description='omnidirectional')
            mic_pos26=Receivers(location=rcv_file['room_'+str(i)]['mic_pos_26'][j],orientation=[0,0,0], description='omnidirectional')
            mic_pos27=Receivers(location=rcv_file['room_'+str(i)]['mic_pos_27'][j],orientation=[0,0,0], description='omnidirectional')
            mic_pos28=Receivers(location=rcv_file['room_'+str(i)]['mic_pos_28'][j],orientation=[0,0,0], description='omnidirectional')
            mic_pos29=Receivers(location=rcv_file['room_'+str(i)]['mic_pos_29'][j],orientation=[0,0,0], description='omnidirectional')
            mic_pos30=Receivers(location=rcv_file['room_'+str(i)]['mic_pos_30'][j],orientation=[0,0,0], description='omnidirectional')
            '''
            setup = RoomSetup(sources=noise_source, receivers=[mic_pos1,mic_pos2] , options=option_list["simulation_params"], room=conf_room_setup['room_'+str(i)])
        #noise_setup = RoomSetup(sources=noise_source, receivers=[mic_pos1,mic_pos2,mic_pos3,mic_pos4,mic_pos5,mic_pos6,mic_pos7,mic_pos8,mic_pos9,mic_pos10,mic_pos11,mic_pos12,mic_pos13,mic_pos14,mic_pos15,mic_pos16,mic_pos17,mic_pos18,mic_pos19,mic_pos20,mic_pos21,mic_pos22,mic_pos23,mic_pos24,mic_pos25,mic_pos26,mic_pos27,mic_pos28,mic_pos29,mic_pos30], options=option_list["simulation_params"], room=conf_room_setup['room_'+str(i)])

        #tmp_rir[j*2:(j+1)*2,:]=generate(setup.get_config())

        #noise_rir[j*2:(j+1)*2,:]=generate(noise_setup.get_config())
            tmp_=generate(setup.get_config()) #replace tmp_ with k
        #tmp_=np.empty((2,20000))
        #for l in range(2):
            #tmp_[l,:]=signal.decimate(k[l,:],int(round(48000/16000)))
            print(tmp_.shape)
            return tmp_

            #np.save("roomsim_011111_48khz.npy",tmp_)


    #Downsample signal
    #downsample_tmp_rir=np.empty((10,20000)) # 1,25 sec signal will have 20000 samples at 16,000 khz sampling frequency
    #downsample_noise_rir=np.empty((2,20000))
    #for k in range(no_of_view_points*2):
        #downsample_tmp_rir[k,:] = signal.decimate(tmp_rir[k,:],int(round(48000/16000)))
        #downsample_noise_rir[k,:]= signal.decimate(noise_rir[k,:],int(round(48000/16000)))
    #rir_id.create_dataset('rir',(10,20000), data=downsample_tmp_rir)
    #rir_noise_id.create_dataset('noise_rir',(2,20000),data=downsample_noise_rir)

x=cal_roomsim([1.991,4.498,1.424])
plt.plot(np.arange(x.shape[1]),x[0,:])
plt.savefig("rir_test_for_diff_repo.png")



print(x.shape)
