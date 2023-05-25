package dp.system.back.services;

import dp.system.back.exceptions.ReservationNotFoundException;
import dp.system.back.exceptions.UserNotFoundException;
import dp.system.back.models.User;
import dp.system.back.repositories.UserRepository;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;

import java.util.List;

@Service
public class UserService {
    private final UserRepository userRepository;

    @Autowired
    public UserService(UserRepository userRepository) {
        this.userRepository = userRepository;
    }

    public String findReservationByPlateNumber(String plateNumber) {

        User user = userRepository.findByPlateNumber(plateNumber);
        if (user == null)
            throw new UserNotFoundException("Unable to find a user for the provided plate number: " + plateNumber + ".");

        String reservedParkingSpaceNumber = user.getReservedParkingSpaceNumber();
        if (reservedParkingSpaceNumber != null) {
            return reservedParkingSpaceNumber;
        } else {
            throw new ReservationNotFoundException("Unable to find a reservation for the provided plate number: " + plateNumber + ".");
        }
    }

    public void resetReservations() {
        List<User> allUsers = userRepository.findAll();
        for (User user : allUsers) {
            user.setReservedParkingSpaceNumber(null);
        }
        userRepository.saveAll(allUsers);
    }

    public void deleteReservationForPlateNumber(String plateNumber) {
        User user = userRepository.findByPlateNumber(plateNumber);
        user.setReservedParkingSpaceNumber(null);
        userRepository.save(user);
    }

}
