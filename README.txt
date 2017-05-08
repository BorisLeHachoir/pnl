# pnl
Projet - Invite de commande pour le noyau linux

Nicolas	Guittonneau	3604340
Erwan	Lenormand	3300358	
Robin	Blottière--Mayo	3200248


I- Mode d'emploi :

##### Sur votre machine ######
Assurez-vous que votre dossier /tmp contienne le fichier linux-4.2.3.
Placez-vous à la racine du projet_pnl puis executez la commande :
./init.sh

##### Sur la VM ######
Uttilisez le mots de passe root pour vous connecter à la machine virtuelle.
Une fois dans la machine virtuelle uttilisez la commande scp pour charger les
fichiers mod.ko et exec puis executez la commande :
insmod mod.ko

Uttilisez les informations s'affichant dans le terminal au chargement du module
pour connaître les paramètres d'exécution de la commande :
mknod /dev/temp c 255 0

Lancez l’invite de commande avec :
./exec


II- Exemple de traces :
[root@vm-nmv ~]# sleep 1000 &
[1] 276
[root@vm-nmv ~]# sleep 1000 &
[2] 277
[root@vm-nmv ~]# sleep 20 &
[3] 278
[root@vm-nmv ~]# ./projet/bin/exec 
[ TOOL ] <-------- TOOL -------->
[ TOOL ] Enter a command, or type "help"
$> help
[ TOOL ] <-------- HELP -------->
[ TOOL ] - list
[ TOOL ] - fg <id>
[ TOOL ] - kill <signal> <pid>
[ TOOL ] - wait <pid> [<pid> ...]
[ TOOL ] - meminfo
[ TOOL ] - modinfo <name>
[ TOOL ] - help
[ TOOL ] - quit
[ TOOL ] <---------------------->
&> wait 277 278
[ TOOL ] wait called with: 2 pids
[ TOOL ] <------------ Wait ---------->
[ TOOL ] Process 278 terminated with 0
$> kill 9 276
[ TOOL ] kill called with arg: 9, 276
[ TOOL ] <----------- Kill ----------->
[ TOOL ] Process was successfully killed
$> meminfo
[ TOOL ] <----- Memory information --->
[ TOOL ] MemTotal: 511253 kB
[ TOOL ] MemFree:  	493104 kB
[ TOOL ] MemShare: 2145 kB
[ TOOL ] Buffers:  	2006 kB
[ TOOL ] SwapTotal:	0 kB
[ TOOL ] SwapFree: 0 kB
[ TOOL ] HighTotal:	0 kB
[ TOOL ] HighFree: 0 kB
[ TOOL ] MemUnit:  	4096 B
$> modinfo mod
[ TOOL ] <---- Module information ---->
[ TOOL ] name: mod
[ TOOL ] version: 0.5.4
[ TOOL ] base addr: 0xffffffffa0000000
[ TOOL ] args: 
&>list
[ TOOL ] <------------ List ---------->
[ TOOL ] There is currently 0 async commands
$> meminfo &
[ TOOL ] meminfo async called
&>list
[ TOOL ] <------------ List ---------->
[ TOOL ] There is currently 1 async commands
[ TOOL ] [5] meminfo &
$> quit
[ TOOL ] exit...
[3]+  Done                    sleep 20
[root@vm-nmv ~]#



III- Ce qui a été fait :
- Toutes les commandes fonctionnes en mode synchrone.
- Les commandes fg, kill, wait, meminfo, modinfo s’exécutent en mode
asynchrone.


IV- Ce qui ne fonctionne pas :
- Il subsiste un segfault que nous n’avons pas pu identifier, qui se produit
de temps en temps avec la commande list en mode asynchrone.