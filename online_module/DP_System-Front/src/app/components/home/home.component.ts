import { HttpErrorResponse } from '@angular/common/http';
import { Component, OnInit } from '@angular/core';
import { HttpClient } from '@angular/common/http';

@Component({
  selector: 'app-home',
  templateUrl: './home.component.html',
  styleUrls: ['./home.component.css']
})
export class HomeComponent implements OnInit {
  eventSource!: EventSource;
  eventData!: string;

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
}

interface YourObject {
  someData: string;
}


