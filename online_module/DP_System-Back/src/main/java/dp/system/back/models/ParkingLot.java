package dp.system.back.models;

import java.util.List;

public class ParkingLot {

    private List<Integer> parkingSpaces;

    public ParkingLot(List<Integer> parkingSpaces) {
        this.parkingSpaces = parkingSpaces;
    }

    public List<Integer> getParkingSpaces() {
        return parkingSpaces;
    }

    public void setParkingSpaces(List<Integer> parkingSpaces) {
        this.parkingSpaces = parkingSpaces;
    }
}
