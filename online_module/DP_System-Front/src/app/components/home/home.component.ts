import { HttpErrorResponse } from '@angular/common/http';
import { Component, OnInit } from '@angular/core';
import { User } from 'src/app/entities/user';
import { LoginService } from 'src/app/services/login.service';
import { ReservationService } from 'src/app/services/reservation.service';
import { UserService } from 'src/app/services/user.service';

@Component({
  selector: 'app-home',
  templateUrl: './home.component.html',
  styleUrls: ['./home.component.css']
})
export class HomeComponent implements OnInit {
  eventSource!: EventSource;
  eventData!: string;
  currentUser: User;
  parkingSpots: boolean[] = [];
  errorMessage: string | null = null;
  successMessage: string | null = null;
  wasNotReservationMade!: boolean;
  currentIndex: number | undefined;
  loginService: any;

  constructor(private reservationService: ReservationService, private userService: UserService, loginService: LoginService) {
    this.currentUser = this.userService.getCurrentUser();
    if(this.currentUser.reservedParkingSpaceNumber != null) {
      this.wasNotReservationMade = false;
    }

    for (let i = 0; i < 16; i++) {
      this.parkingSpots.push(false);
    }
  }

  ngOnInit(): void {
    this.eventSource = new EventSource('http://localhost:8083/sse');

    this.eventSource.addEventListener('message', (event: MessageEvent) => {
      console.log('Received event:', event.data);

      const parkingSpotData = JSON.parse(event.data);
      console.log(parkingSpotData);
      for (let i = 0; i < 16; i++) {
        this.parkingSpots[i] = parkingSpotData[i];
      }
      console.log(this.parkingSpots);
    });

    this.eventSource.addEventListener('error', (error: Event) => {
      console.error('EventSource error:', error);
    });
  }

  ngOnDestroy(): void {
    this.eventSource.close();
  }

  public onAddReservation(): void {
    this.reservationService.addReservation(this.currentUser).subscribe(
      (response) => {
        console.log('Reservation added successfully');
        this.wasNotReservationMade = false;
      },
      (error: HttpErrorResponse) => {
        console.error('Failed to add reservation!');
      }
    );
  }

  toggleParkingSpotState(index: number): void {
    this.parkingSpots[index] = !this.parkingSpots[index];
  }

  logout(): void {
    this.loginService.logout();
  }
}


