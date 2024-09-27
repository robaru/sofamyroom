# from pyroomsim import py_mul_struct
# print(py_mul_struct(1, 4.2))


# from pyroomsim import make_sources
#
# hey = dict(first=1, second=2.0)
# hey2 = dict(first=12, second=22.0)
#
# s = make_sources(hey, hey2)
# print(s[0])

# WORKS
# from roomsim import make_options
# from api import Options
# options = Options()
# # For options, we'll need the get config, starts to be too much.
# hey = make_options(options.get_config())
# print(hey)

# from roomsim import make_surface
# from api import Surface
#
# surf = Surface()
# make_surface(surf.get_config())
# print(surf.get_config())


# WORKS
# from roomsim import make_room
# from api import Room
#
# room = Room()
# make_room(room.get_config())

# # Worked
# from api import RoomSetup, Sources, Receivers
# from roomsim import make_sensors
# # sources = Sources(location=(8, 2.5, 1.6), orientation=(180, 0, 0), description="subcardiod")
# sources = [
#     Sources(location=(8, 2.5, 1.6), orientation=(180, 0, 0), description="subcardiod"),
#     Sources(location=(2, 3.3, 2.6), orientation=(0, 180, 0), description="subcardiod"),
# ]
#
# make_sensors([sources[0].get_config(), sources[1].get_config()])


from pysofamyroom.internal import make_roomsetup, generate
from pysofamyroom.api import RoomSetup, Sources, Receivers
import h5py
import numpy as np
from scipy import signal
from tqdm import tqdm


'''
sources = [
    Sources(location=(8, 2.5, 1.6), orientation=(180, 0, 0), description="subcardioid"),
    Sources(location=(8, 3, 2), orientation=(0, 180, 0), description="subcardioid"),
]
'''


#For the moment, it's not a neat way to load the file; have to work on this a bit.

import yaml
with open('conf_sim_params_2.yml') as file_c,open('conf_room_setup_2.yml') as file_1 , open('conf_receivers_2.yml') as file_2,open('conf_source_2.yml') as file_3,open('conf_noise_source_2.yml') as file_4:
    option_list=yaml.load(file_c,Loader=yaml.FullLoader)
    conf_room_setup=yaml.load(file_1,Loader=yaml.FullLoader)
    rcv_file=yaml.load(file_2,Loader=yaml.FullLoader)
    src_file=yaml.load(file_3,Loader=yaml.FullLoader)
    src_noise_file=yaml.load(file_4,Loader=yaml.FullLoader)



print(option_list['simulation_params'])
print("------------------")
print(conf_room_setup['room_0'])
print("------------------")
print(rcv_file['room_0']['mic_pos_1'][0])
print("------------------")
print(src_file['room_0']['source_pos'][0])
print("===================")

no_of_rooms=20000
no_of_view_points=5

Output_file=h5py.File("/srv/storage/talc3@talc-data.nancy/multispeech/calcul/users/psrivast/generated_rirs_lwh_correct_noise.hdf5",'w')
#Output_file=h5py.File("generated_r.hdf5",'w')


Output_file.attrs['fs'] = option_list['simulation_params']['fs']
Output_file.attrs['response_duration'] = option_list['simulation_params']['responseduration']
Output_file.attrs['bandsperoctave'] = option_list['simulation_params']['bandsperoctave']
Output_file.attrs['referencefrequency'] = option_list['simulation_params']['referencefrequency']
Output_file.attrs['airabsorption'] = option_list['simulation_params']['airabsorption']
Output_file.attrs['distanceattenuation'] = option_list['simulation_params']['distanceattenuation']
Output_file.attrs['subsampleaccuracy'] = option_list['simulation_params']['subsampleaccuracy']
Output_file.attrs['highpasscutoff'] = option_list['simulation_params']['highpasscutoff']
Output_file.attrs['verbose'] = option_list['simulation_params']['verbose']
Output_file.attrs['simulatespecular'] = option_list['simulation_params']['simulatespecular']
Output_file.attrs['reflectionorder'] =  option_list['simulation_params']['reflectionorder']
Output_file.attrs['simulatediffuse'] =  option_list['simulation_params']['simulatediffuse']
Output_file.attrs['numberofrays'] =  option_list['simulation_params']['numberofrays']
Output_file.attrs['diffusetimestep'] =  option_list['simulation_params']['diffusetimestep']
Output_file.attrs['rayenergyfloordB'] =  option_list['simulation_params']['rayenergyfloordB']
Output_file.attrs['uncorrelatednoise'] =  option_list['simulation_params']['uncorrelatednoise']

#room_save=Output_file.create_group("room_config")
#rcv_save=Output_file.create_group("receiver_config")
#source_save=Output_file.create_group("source_config")
#rirs_save=Output_file.create_group('rirs')
rirs_noise=Output_file.create_group('rirs_noise')
#room_save.attrs['saftey_distance'] = 0.3
#room_save.attrs['reference_freq'] = 125
#room_save.attrs['no_of_receiver_per_room'] = 5

for i in tqdm(range(no_of_rooms)):
    #room_id=room_save.create_group('room_' + str(i))
    #rcv_id=rcv_save.create_group('room_'+str(i))
    #src_id=source_save.create_group('room_'+str(i))
    #rir_id=rirs_save.create_group('room_'+str(i))
    rir_noise_id=rirs_noise.create_group('room_'+str(i))

    #room_id.create_dataset('dimension',3,data=conf_room_setup['room_'+str(i)]['dimension'])
    #room_id.create_dataset('humidity',1,data=conf_room_setup['room_'+str(i)]['humidity'])
    #room_id.create_dataset('temprature',1,data=conf_room_setup['room_'+str(i)]['temperature'])
    #room_id.create_dataset('frequency',6,data=[125 * pow(2, a) for a in range(6)])
    #room_id.create_dataset('absorption',36,data=conf_room_setup['room_'+str(i)]['surface']['absorption'])
    #room_id.create_dataset('diffusion',36,data=conf_room_setup['room_'+str(i)]['surface']['diffusion'])

    #rcv_id.create_dataset('barycenter',(5,3),data=rcv_file['room_'+str(i)]['barycenter'])
    #rcv_id.create_dataset('barycenter_ypr', (5, 3), data=rcv_file['room_'+str(i)]['barycenter_ypr'])
    #rcv_id.create_dataset('mic_pos_1',(5,3),data=rcv_file['room_'+str(i)]['mic_pos_1'])
    #rcv_id.create_dataset('mic_pos_2',(5,3),data=rcv_file['room_'+str(i)]['mic_pos_2'])

    #rcv_id.create_dataset('mic_pos_3', (5, 3), data=li_rec_3)
    #rcv_id.create_dataset('mic_pos_4', (5, 3), data=li_rec_4)

    #src_id.create_dataset('source_pos',(5,3),data=src_file['room_'+str(i)]['source_pos'])
    #src_id.create_dataset('source_ypr', (5, 3), data=src_file['room_'+str(i)]['source_ypr'])
    #src_id.create_dataset('source_description', (5, 1), data=src_file['room_'+str(i)]['source_description'])
    tmp_rir=np.empty((10,60000))
    noise_rir=np.empty((10,60000))
    for j in range(no_of_view_points):
        #sources=Sources(location=src_file['room_'+str(i)]['source_pos'][j], orientation=src_file['room_'+str(i)]['source_ypr'][j], description=src_file['room_'+str(i)]['source_description'][j][0])
        noise_source=Sources(location=src_noise_file['room_'+str(i)]['source_pos'][j], orientation=src_noise_file['room_'+str(i)]['source_ypr'][j], description=src_noise_file['room_'+str(i)]['source_description'][j][0])
        mic_pos1=Receivers(location=rcv_file['room_'+str(i)]['mic_pos_1'][j],orientation=[0,0,0], description='omnidirectional')
        mic_pos2=Receivers(location=rcv_file['room_'+str(i)]['mic_pos_2'][j],orientation=[0,0,0], description='omnidirectional')
        #setup = RoomSetup(sources=sources, receivers=[mic_pos1,mic_pos2] , options=option_list["simulation_params"], room=conf_room_setup['room_'+str(i)])
        noise_setup = RoomSetup(sources=noise_source, receivers=[mic_pos1,mic_pos2] , options=option_list["simulation_params"], room=conf_room_setup['room_'+str(i)])

        #tmp_rir[j*2:(j+1)*2,:]=generate(setup.get_config())
        noise_rir[j*2:(j+1)*2,:]=generate(noise_setup.get_config())


    #Downsample signal
    #downsample_tmp_rir=np.empty((10,20000)) # 1,25 sec signal will have 20000 samples at 16,000 khz sampling frequency
    downsample_noise_rir=np.empty((10,20000))
    for k in range(no_of_view_points*2):
        #downsample_tmp_rir[k,:] = signal.decimate(tmp_rir[k,:],int(round(48000/16000)))
        downsample_noise_rir[k,:]= signal.decimate(noise_rir[k,:],int(round(48000/16000)))
    #rir_id.create_dataset('rir',(10,20000), data=downsample_tmp_rir)
    rir_noise_id.create_dataset('noise_rir',(10,20000),data=downsample_noise_rir)




'''
sources = Sources(location=src_file['src_0']['location'], orientation=src_file['src_0']['orientation'], description=src_file['src_0']['description'])

receivers=[]
no_of_rcv=2
#Just testing out stuff with two different receiver for one single room setup
for n in range(no_of_rcv):
    receivers.append(Receivers(location=rcv_file['rcv_'+str(n)]['location'],orientation=rcv_file['rcv_'+str(n)]['orientation'], description=rcv_file['rcv_'+str(n)]['description']))

setup = RoomSetup(sources=sources, receivers=receivers , options=option_list["simulation_params"], room=conf_room_setup[0]['room_0'])
print(setup.get_config())


rirs_np_arr=generate(setup.get_config())
'''



#setup.write_wavefile(fs,rirs_np_arr,"wav_id")

#make_roomsetup(setup.get_config())
#hey = setup.get_config()
#ok = RoomSetup.from_config(hey)


# yaml.safe_dump(hey, open("conf2.yml", "w"), default_flow_style=None)

#setup.to_yaml("conf.yml")
# import ipdb; ipdb.set_trace()
# new = RoomSetup.from_yaml("conf.yml")
#setup = RoomSetup.from_txt("sampleroomsetup.txt")
#print(setup.options.get_config())
