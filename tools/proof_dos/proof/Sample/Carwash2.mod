BEGIN;
         CREATE:EX(1,1);
         COUNT:2,1;
         ASSIGN:A(1)=NC(2);
         WRITE,1,"('TIME ',F10.4)":TNOW;              
         WRITE,1,"('CREATE Car ',F5.0)":A(1);
         WRITE,1,"('PLACE ',F5.0,' ON ENTRY')":A(1);
         DELAY:.25;
         QUEUE,1;
         SEIZE:CHAIN;
         WRITE,1,"('TIME ',F10.4)":TNOW;              
         WRITE,1,"('PLACE ',F5.0, 'ON CHAIN')":A(1);
         DELAY:2;
         RELEASE:CHAIN;
         DELAY:1.5;
         WRITE,1,"('TIME ',F10.4)":TNOW;              
         WRITE,1,"('PLACE ',F5.0,' ON EXIT')":A(1);
         DELAY:.25;
         WRITE,1,"('TIME ',F10.4)":TNOW;              
         WRITE,1,"('DESTROY ',F5.0)":A(1);
         COUNT:1,1:DISPOSE;  
END;


