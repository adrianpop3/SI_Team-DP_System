import { HttpErrorResponse } from '@angular/common/http';
import { Component } from '@angular/core';
import { NgForm } from '@angular/forms';
import { ReservationService } from 'src/app/services/reservation.service';

@Component({
  selector: 'app-home',
  templateUrl: './home.component.html',
  styleUrls: ['./home.component.css']
})
export class HomeComponent {
    isFree: boolean | undefined;
    isUserloggedIn: boolean | undefined;

    constructor(private reservationService: ReservationService) {}

    public onAddReservation(addForm: NgForm): void {
      document.getElementById('add-reservation-form')?.click();
      this.reservationService.addReservation(addForm.value).subscribe(
        (response) => {
          addForm.reset()
        },
        (error: HttpErrorResponse) => {
          alert(error.message);
          addForm.reset()
        }
      );
    }
}
