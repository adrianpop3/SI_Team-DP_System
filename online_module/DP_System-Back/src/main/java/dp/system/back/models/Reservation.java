package dp.system.back.models;

import java.io.Serializable;

public class Reservation implements Serializable {

    private String plateNumber;
    private String parkingSpaceNumber;

    public Reservation(String plateNumber, String parkingSpaceNumber) {
        this.plateNumber = plateNumber;
        this.parkingSpaceNumber = parkingSpaceNumber;
    }

    public String getPlateNumber() {
        return plateNumber;
    }

    public String getParkingSpaceNumber() {
        return parkingSpaceNumber;
    }

    public void setPlateNumber(String plateNumber) {
        this.plateNumber = plateNumber;
    }

    public void setParkingSpaceNumber(String parkingSpaceNumber) {
        this.parkingSpaceNumber = parkingSpaceNumber;
    }
}
