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



III- Ce qui a été fait :



IV- Ce qui ne fonctionne pas :
