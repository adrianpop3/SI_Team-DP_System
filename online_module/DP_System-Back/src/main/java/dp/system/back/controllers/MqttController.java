package dp.system.back.controllers;

import org.springframework.scheduling.annotation.EnableScheduling;
import org.springframework.stereotype.Controller;

@Controller
@EnableScheduling
public class MqttController {

//    @Autowired
//    private MessageChannel mqttOutputChannel;
//
//    @Scheduled(fixedDelay = 1000) // send message every second
//    public void sendMessage() {
//
//        String payload = "Spring boot is sending a message!";
//        Message<String> message = MessageBuilder.withPayload(payload).build();
//        mqttOutputChannel.send(message);
//    }
}
