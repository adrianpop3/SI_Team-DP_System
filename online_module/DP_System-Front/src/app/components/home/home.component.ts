import { HttpErrorResponse } from '@angular/common/http';
import { Component, OnInit } from '@angular/core';
import { HttpClient } from '@angular/common/http';
import { User } from 'src/app/entities/user';
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

  constructor(private reservationService: ReservationService, private userService: UserService) {
    this.currentUser = this.userService.getCurrentUser();

    for (let i = 0; i < 16; i++) {
      this.parkingSpots.push(false);
    }
  }

  ngOnInit(): void {
    this.eventSource = new EventSource('http://localhost:8083/sse'); // Replace with your SSE endpoint URL

    this.eventSource.addEventListener('message', (event: MessageEvent) => {
      console.log('Received event:', event.data);

      // const yourObject: YourObject = JSON.parse(event.data);
      // this.eventData = yourObject.someData;
    });

    this.eventSource.addEventListener('error', (error: Event) => {
      console.error('EventSource error:', error);
      // Handle the error if necessary
    });
  }

  ngOnDestroy(): void {
    this.eventSource.close(); // Close the EventSource connection when the component is destroyed
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

  toggleParkingSpotState(index: number): void {
    this.parkingSpots[index] = !this.parkingSpots[index];
  }
}

interface YourObject {
  someData: string;
}


