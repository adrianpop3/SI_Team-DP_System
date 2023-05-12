package dp.system.back.services;

import dp.system.back.exceptions.ReservationNotFoundException;
import dp.system.back.models.Reservation;
import dp.system.back.repositories.ReservationRepository;
import jakarta.transaction.Transactional;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;

import java.util.List;

@Service
public class ReservationService {
    private final ReservationRepository reservationRepository;

    @Autowired
    public ReservationService(ReservationRepository reservationRepository) {
        this.reservationRepository = reservationRepository;
    }

    public Reservation addReservation (Reservation reservation) {
        return reservationRepository.save(reservation);
    }

    public Reservation findReservationById(Long id) {
        return reservationRepository.findReservationById(id).orElseThrow(
                () -> new ReservationNotFoundException("Reservation with id " + id +" was not found."));
    }

    public List<Reservation> findAllReservations() {
        return reservationRepository.findAll();
    }

    public Reservation updateReservation(Reservation reservation) {
        return reservationRepository.save(reservation);
    }

    @Transactional
    public void deleteReservation(Long id) {
        reservationRepository.deleteReservationById(id);
    }
}
