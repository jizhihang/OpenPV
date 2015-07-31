#Generate new key
ssh-keygen -t rsa -f clusterkey -q -N ""

#Grab all lines with node in the name
ips=$(cat ./hosts | awk '/node/ {print $1}')
nodenames="$(cat ./hosts | awk '/node/ {print $2}')"

#For each ip address
for ip in $ips
do
   if test -z "$(ssh-keygen -F $ip)"; then
      $(ssh-keyscan $ip >> ~/.ssh/known_hosts)
   fi

   #Copy hosts file to home directory
   scp ./hosts ec2-user@$ip:~/
   scp ./clusterkey ec2-user@$ip:~/
   scp ./clusterkey.pub ec2-user@$ip:~/
   #Copy node file to home directory
   scp ./nodefile ec2-user@$ip:~/
   scp ./cpCluster.sh ec2-user@$ip:~/

   #Copy to proper places with sudo
   ssh -t ec2-user@$ip 'sudo mv ~/hosts /etc/hosts; sudo mv ~/clusterkey ~/.ssh/id_rsa; cat ~/clusterkey.pub >> ~/.ssh/authorized_keys'

   #Put all keys to known_hosts
   for name in $nodenames
   do
      ssh ec2-user@$ip 'ssh-keyscan '$name' >> ~/.ssh/known_hosts' 
   done

   #Update and build petavision
   ssh -t ec2-user@$ip 'cd ~/workspace/PetaVision; svn update; cd ~/workspace/; cmake -DCMAKE_BUILD_TYPE=Release -DCUDA_GPU=True -DCUDA_RELEASE=True -DCUDNN=True -DCUDNN_PATH=~/cuDNN/cudnn-6.5-linux-x64-R2-rc1 -DOPEN_MP_THREADS=True -DPV_DIR=~/workspace/PetaVision; cd ~/workspace/PetaVision; make -j 8'
done