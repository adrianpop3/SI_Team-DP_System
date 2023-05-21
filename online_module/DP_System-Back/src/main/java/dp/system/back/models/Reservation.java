package dp.system.back.models;

public class Reservation {
    private String plateNumber;
    private String parkingSpaceNumber;

    public Reservation(String plateNumber, String parkingSpaceNumber) {
        this.plateNumber = plateNumber;
        this.parkingSpaceNumber = parkingSpaceNumber;
    }
}
