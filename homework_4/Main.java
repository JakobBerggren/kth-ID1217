public class Main{
    public Main(){
        FuelSpaceStation fss = new FuelSpaceStation(v, n, q);
        RunnableThread ve = new RunnableThread(fss, false);
        RunnableThread rf = new RunnableThread(fss, true);

        Thread[] vehicles = new Thread[10];

        for(int i = 0; i< vehicles.length;i++)
            vehicles[i] = new Thread(ve);
        Thread refiller = new thread(re);

        for(int i = 0; i< vehicles.length;i++)
            vehicles[i].start();
        refiller.start();
    }

    public static void main(String[] args) {
        
        new Main(args[0],args[1], args[2]);
    }
}

public class RunnableThread implements Runnable{
    FuelSpaceStation fss;
    int nitrogenTank, quantumTank;

    boolean refill;
    
    public RunnableThread(FuelSpaceStation fss, boolean refill){
        this.fss = fss;
        this.refill = refill;
        if(refill){
            this.nitrogenTank = 10000;
            this.quantumTank = 10000;
        }
        else{
            this.nitrogenTank = Math.random()*100;
            this.quantumTank = Math.random()*100;
        }
    }

    public void run(){
        if(refill){
            while(true){
                System.out.println("were getting this far boys");
                fss.dock();
                fss.refillNitrogen(nitrogenTank);
                fss.refillQuantum(quantumTank);
                fss.undock();

                Thread.sleep((long) ((int) Math.random()*10000 + 2000));
            }
        }
        else{
            
            while(true){
            fss.dock();
            fss.fillNitrogen(nitrogenTank);
            fss.fillQuantum(quantumTank);
            fss.undock();

            Thread.sleep((long) Math.random()*10000 + 2000);
            }
        }    
    }
}