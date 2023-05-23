import { HttpErrorResponse } from '@angular/common/http';
import { Component } from '@angular/core';
import { User } from 'src/app/entities/user';
import { ReservationService } from 'src/app/services/reservation.service';
import { UserService } from 'src/app/services/user.service';

@Component({
  selector: 'app-home',
  templateUrl: './home.component.html',
  styleUrls: ['./home.component.css']
})
export class HomeComponent {
    currentUser: User;

    constructor(private reservationService: ReservationService, private userService: UserService) {
      this.currentUser = this.userService.getCurrentUser();
    }

    public onAddReservation(): void {
      this.reservationService.addReservation(this.currentUser).subscribe(
        (response) => {
          console.log('Reservation added successfully');
        },
        (error: HttpErrorResponse) => {
          alert(error.message);
          console.error('Failed to add reservation!');
        }
      );
    }
}
