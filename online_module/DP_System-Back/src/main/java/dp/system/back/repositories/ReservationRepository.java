package dp.system.back.repositories;

import dp.system.back.models.Reservation;
import org.springframework.data.jpa.repository.JpaRepository;

import java.util.Optional;

public interface ReservationRepository extends JpaRepository<Reservation, Long> {

    void deleteReservationById(Long id);

    Optional<Reservation> findReservationById(Long id);
}
